# GLIM SW 과제 - Step 1: CLI Image Processor

이 저장소는 std::thread와 SIMD 병렬화를 이용한 CLI 기반 이미지 처리 애플리케이션입니다.
Windows 11 + Visual Studio 2022 환경을 대상으로 작성되었으며,
GitHub Actions `windows-latest`에서 자동 빌드되며, 매 푸시마다 Windows 실행 파일이 GitHub Release에 게시됩니다.

---

## 1. 최적화 가이드라인

본 프로젝트에서 적용한 최적화는 다음 원칙을 따릅니다.

### 준거
- **메모리 트래픽 우선 감소**: 데이터 이동보다 연산을 늘리는 것이 빠른 경우가 많습니다.
- **캐시 지역성 향상**: 순차적 메모리 접근, 연속된 버퍼 사용, 작은 타일 단위 처리를 선호합니다.
- **불필요한 할당 제거**: 루프 내 동적 할당을 금지하고, 중간 버퍼를 재사용합니다.
- **루프 내 중복 연산 제거**: 불변 계산을 루프 밖으로 호이스팅합니다.
- **정수 고정소수점 우선**: 출력 품질이 동일하다면 부동소수점 대신 정수 스케일 누적 후 정규화를 사용합니다.
- **SIMD 친화적 메모리 레이아웃**: 병렬 작업, 2^n bytes 단위 Align 친화 작업에 특화된 SIMD를 이용하기 위해 채널 분리 후 32byte 정렬 로드/스토어를 활용합니다.
- **정적 작업 분할**: 균등한 workload에는 동적 스케줄링 대신 정적 행 블록 분할을 사용합니다.
- **Lock-free 병렬화**: 입력은 읽기 전용, 출력은 각 스레드가 분리된 영역에 기록하여 뮤텍스를 피합니다.
- **분리 가능한 커널 탐지**: 3x3 box blur처럼 분리 가능한 커널은 1D horizontal + vertical 두 패스로 나눕니다.

### 장점
- **예측 가능한 성능**: 정적 분할과 단순한 루프는 오버헤드가 적고 확장성이 좋습니다.
- **메모리 대역폭 절약**: 병렬적으로 작업을 변환 후 SIMD 처리는 캐시 라인 활용률을 높입니다.
- **낮은 지연 시간**: 애초에 자원을 정적 행 기반으로 분리합니다. 이러한 락이 필요없는 설계로 스레드 간 경합이 없습니다.
- **품질 보존**: 정수 누적 + 클램핑은 부동소수점 변환보다 빠륩니만 결과가 동일합니다.

### 단점
- **코드 복잡도 증가**: SIMD intrinsic과 planar 변환 로직은 전통적인 스칼라 코드보다 길고 디버깅이 어렵습니다.
- **플랫폼 의존성**: AVX2 코드는 x86_64 전용이며, 구형 CPU에서는 전통적인 스칼라 폭백이 작동합니다.
- **메모리 사용량 증가**: planar 중간 버퍼가 3개 추가로 필요합니다.
- **세밀한 튜닝 필요**: 행 블록 크기나 타일 크기는 CPU 캐시 크기에 따라 달라질 수 있습니다.

### 써야 할 때
- 고해상도 이미지(수백만 픽셀 이상)의 실시간 필터링이 필요할 때
- 동일한 커널이 여러 이미지에 반복 적용될 때
- 병렬 처리가 가능한 독립적인 행/타일 단위 작업일 때
- 출력 품질을 유지하면서도 처리량을 극대화해야 할 때

### 말아야 할 때
- 이미지 크기가 매우 작아서 병렬화 오버헤드가 이득보다 클 때
- 타겟 플랫폼이 고정되지 않아 portable해야 할 때 (SIMD 버전 관리 부담)
- 코드 가독성과 유지보수성이 절대적으로 우선일 때
- 알고리즘 자체가 메모리 접근 패턴이 불규칙해 SIMD 효율이 떨어질 때

---

## 2. 과제 개요

24비트 무압축 BMP 파일을 처리하는 명령줄 도구입니다.

### 구현 기능
- **필터**: Grayscale, Threshold(파라미터화), Blur(3x3 box), Sharpen(3x3)
- **기하 변환**: Crop, Resize(양선형 보간), Flip(상하/좌우)
- **파이프라인**: `--pipeline "grayscale,blur,threshold:128"` 형식으로 다중 필터 체인 지원
- **히스토그램**: `--histogram` 옵션으로 콘솔 ASCII 그래프 출력
- **로깅**: `--log` 옵션으로 타임스탬프 파일 출력
- **병렬 처리**: `std::thread` 행 블록 분할 + AVX2 SIMD (planar convolution)

### 아키텍처
- **전략 패턴(Strategy Pattern)**: 모든 필터는 `FilterBase` 추상 클래스를 상속받습니다.
- **빌더 패턴(Builder Pattern)**: `FilterPipelineBuilder`가 순차 필터 체인을 구성합니다.

---

## 3. 빌드 및 릴리스

모든 실행 파일은 GitHub Actions `windows-latest` + Visual Studio 2022에서 자동 빌드됩니다.
매 `push`마다 아래 실행 파일이 [Releases](https://github.com/gg582/glim-sw-test/releases)의 `latest` 태그에 업로드됩니다.

- `ImageProcessor.exe`

### 로컬 빌드 (Visual Studio 2022)

```powershell
cd step-1
cmake -B build -S . -G "Visual Studio 17 2022"
cmake --build build --config Release
```

---

## 4. 실행 예시

```powershell
# 단일 필터
ImageProcessor.exe --input in.bmp --output out.bmp --filter grayscale

# 파이프라인
ImageProcessor.exe --input in.bmp --output out.bmp --pipeline "grayscale,blur,threshold:128"

# 기하 변환 + 히스토그램 + 로그
ImageProcessor.exe --input in.bmp --output out.bmp --crop 10,10,100,100 --resize 50,50 --flip horizontal --histogram --log run.log
```

### 왜 GitHub Actions를 이용하여 빌드하는가?

본 과제의 요구 환경은 Windows 11 + Visual Studio 2022입니다. 따라서 최종 실행 파일은 GitHub Actions의 `windows-latest` 환경에서 Visual Studio 2022/MSVC로 빌드되도록 구성했습니다.

로컬 개발 환경과 무관하게 동일한 빌드 환경에서 반복 검증할 수 있도록 CI 기반 빌드를 사용했습니다. 이를 통해 제출물의 재현성을 높이고, 매 커밋마다 Windows 실행 파일(`.exe`)이 자동 생성되도록 했습니다.

또한 Release의 `latest` 태그에 실행 파일을 자동 업로드하여 채점자가 별도의 빌드 과정 없이 산출물을 확인할 수 있도록 했습니다.

## 왜 한글 주석이 영어로 변환되었는가?

우선, 한글 주석을 사용하기 위해서는 인코딩과 문자 처리 환경에 대한 일정 수준의 합의를 전제로 합니다.

제출자는 채점 환경의 편집기 설정, 문서화 도구, 문자 처리 정책을 알 수 없습니다. 또한 향후 Doxygen 등의 자동 문서화 도구를 사용할 경우에도 문자 인코딩 처리 방식에 따라 결과가 달라질 수 있습니다.

반면 ASCII 범위의 문자는 UTF-8, UTF-16, EUC-KR 등 대부분의 문자 인코딩 체계에서 안정적으로 표현됩니다. 따라서 제출 환경을 예측할 수 없는 상황에서는 ASCII 기반의 기술 문서를 작성하는 것이 가장 보수적인 선택이라고 판단했습니다.

또한 사용된 API, 라이브러리, 컴파일러 진단 메시지 및 공식 문서가 대부분 영어 용어를 기준으로 작성되어 있으므로, 주석 역시 동일한 용어 체계를 유지하도록 정리했습니다.

따라서 본 과제에서는 한글 주석이 부적절하다고 판단한 것이 아니라, 제출물의 장기 보존성과 문서화 일관성을 고려하여 영어 주석을 사용했습니다.
