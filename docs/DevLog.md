# Development Log

이 문서는 하루 작업 종료 시 작성한다.
목표는 "결정과 방향"을 기록하는 것.

---

## 📅 2026-02-21

### 🎯 오늘 목표 (최대 3개)
- 플레이어 체력 UI 구현

---

### 완료한 작업
- ATDPlayerCharacter 상속을 ACharacter → ATDBaseCharacter로 변경
    - HealthComponent 자동 획득, HandleDeath 바인딩 포함
- UTDPlayerStatusHUD와 OnHealthChanged 델리게이트 연결
    - BeginPlay에서 위젯 생성 + 바인딩 + 초기값 동기화
- TestDamage 함수 추가 (P키 → 10 데미지)
    - DefaultInput.ini에 TestDamage 액션 매핑 추가
- ReloadBarWidget 중복 BindWeapon 호출 제거

---

### 발생한 문제
- BindKey(EKeys::P) 사용 시 FInputChord 링커 에러 발생
    - InputCore가 PublicDependencyModuleNames에 있음에도 dllimport 심볼 미해결

---

### 해결 방법 / 결정 사항
- BindKey 대신 DefaultInput.ini에 액션 매핑 추가 후 BindAction으로 변경
    - Fire/Reload와 동일한 방식으로 통일

---

### 미완료 / 보류
- 체력 감소 시 색상 변화 연출
- 피격 시 Vignette/붉은 플래시 연동

---

### 구조적 메모 (선택)
- HealthComponent::BeginPlay()는 Actor::BeginPlay()보다 먼저 실행 → 초기값 동기화 안전
- 플레이어/적 모두 ATDBaseCharacter를 통해 동일한 HealthComponent 구조 공유
- 화면 고정 HUD는 PlayerController 위치 제어 불필요 (Ammo/Reload와 다름)

---

### ▶ 내일 할 일 (최대 3개)
- Muzzle Flash 구현
- Hit Impact 구현

---

---
## 📅 2026-02-20

### 🎯 오늘 목표 (최대 3개)
- WeaponPresetDA에 실제 무기 스탯 추가
- WeaponBase가 Preset 기반으로 스탯 초기화하도록 수정
- 탄약 초기값 및 UI 갱신 흐름 안정화

---

### 완료한 작업
- UTDWeaponPresetDA에 FTDWeaponStats 구조체 추가
- Preset에 FireRate, Damage, Range, SpreadDeg, MagazineSize, ReloadTime 필드 정의
- ATDWeaponBase::SetPartsFromPreset()에서:
    - Preset->Stats 기반으로 무기 스탯 적용
    - MagazineSize 기준으로 AmmoInMag 초기화
    - NotifyAmmoChanged() 호출로 UI 동기화
- 권총 기본 스펙 설정 및 정상 동작 확인

---

### 발생한 문제
- Preset에 Stats를 추가했으나 Preset->SpreadDeg 형태로 직접 접근하여 컴파일 에러 발생

---

### 해결 방법 / 결정 사항
- 모든 스탯 접근을 Preset->Stats.XXX 구조로 수정
- 스탯 초기화는 BeginPlay가 아니라 Preset 적용 시점에서 처리하도록 결정
- 무기 데이터의 단일 진입점은 Preset으로 유지
---

### 미완료 / 보류
- Reload 타이머 실제 동작 검증
- 파츠가 스탯에 영향 주는 구조 여부 미정

---

### 구조적 메모 (선택)
- WeaponPresetDA가 "데이터 소스 단일화 역할"을 담당하도록 구조가 정리됨
- WeaponBase는 Preset을 적용받는 실행 객체 역할로 분리 유지
- 스탯 확장은 Preset 구조 안에서만 진행 가능하도록 설계

---

### ▶ 내일 할 일 (최대 3개)
- 플레이어 체력 UI 구현

---

---
## 📅 2026-02-19

### 🎯 오늘 목표 (최대 3개)
- 재장전 인디케이터 UI 구현
- 재장전 시간과 게이지 동기화
- Weapon ↔ UI 이벤트 구조 확정

---

### 완료한 작업
- ReloadBar 위젯 생성 및 Viewport 추가
- WeaponBase의 OnReloadUIStart / OnReloadUIStop 델리게이트와 위젯 바인딩
- 재장전 Duration 기반 애니메이션 재생 속도 동기화 구현
- ReloadBarWidget 위치를 PlayerController에서 마우스 기반으로 갱신

---

### 발생한 문제
- 재장전 게이지가 실제 재장전 시간보다 빨리 차는 현상
    - Percent 트랙 마지막 키가 0.65초에 위치해 있던 문제 발견
- ReloadBarWidget이 마우스를 따라오지 않음
    - PlayerController에서 AmmoWidget만 위치 갱신하고 있던 버그
---

### 해결 방법 / 결정 사항
- PlayerController는 위치 제어만 담당하도록 구조 유지
- UI 갱신은 Tick 기반이 아닌 Weapon Delegate 이벤트 기반 구조 유지
- 현재는 타이머 기반 재장전, 추후 몽타주 Notify 기반으로 전환 예정

---

### 미완료 / 보류
- 몽타주 기반 재장전 전환
- 재장전 중 인터럽트(피격/구르기 등) 상태 처리
- ReloadBar 페이드 아웃 연출 여부 결정
---

### 구조적 메모 (선택)
- UI 애니메이션과 실제 게임 로직 동기화는 "시간 계산"보다 "구조 정합성"이 중요
- 위젯은 Character에서 생성, 위치 제어는 PlayerController에서 처리하는 책임 분리 유지
- 애니메이션 트랙 키 위치 실수는 체감 속도 왜곡의 주요 원인

---

### ▶ 내일 할 일 (최대 3개)
- 체력 UI 구현 (이벤트 기반 구조 유지)
- 무기 DataAsset으로 실제 수치 이전
- WeaponBase 하드코딩 값 정리 및 DA 참조 구조 정리

---

---

## 📅 2026-02-16

### 🎯 오늘 목표 (최대 3개)
- 동적 머티리얼 기반 탄약 파이 UI 구현
- 무기 시스템과 UI를 델리게이트로 연결
- Ammo Indicator를 마우스 추적형 UI로 확장

---

### 완료한 작업
- 도넛 형태의 Dynamic Material 기반 Ammo Pie Indicator 구현
- 무기 클래스의 OnAmmoChanged 델리게이트를 위젯에 바인딩
- 무기 상태 변경 시 UI가 자동 갱신되도록 이벤트 기반 구조 확정
- Ammo Indicator를 마우스 오른쪽에 위치하도록 PlayerController에서 위치 갱신 처리
- Offset을 Blueprint에서 조절 가능하도록 노출

---

### 발생한 문제
- 델리게이트 바인딩을 Blueprint에서 처리하려고 했을 때 동작하지 않는 현상 발생
    - C++에서 델리게이트를 쏘고, BP에서 바인딩/연결하려 했으나
    - 호출(브로드캐스트) 시점이 BP 바인딩보다 먼저 발생하는 케이스가 생겨 UI 갱신이 누락됨
- Offset 하드코딩으로 BP 값이 적용되지 않는 문제 확인됨
---

### 해결 방법 / 결정 사항
- BP에서 연결/바인딩을 맡기지 않고, C++에서 위젯 생성 + 델리게이트 바인딩까지 전부 세팅하도록 변경
    -> 호출 순서(초기화 타이밍) 문제를 제거하고, 동작을 확실하게 고정
- UI 갱신은 Tick 기반 폴링이 아니라 Weapon Delegate 기반 이벤트 구조로 유지
- 하드코딩 제거 후 MouseOffset 멤버 변수를 사용하도록 통일
- UI는 Character에서 생성, 위치 제어는 PlayerController가 담당하는 구조 확정
---

### 미완료 / 보류
- 화면 가장자리 보정 로직
- 마우스 추적 보간 처리 여부 결정
- Reload Indicator UI 설계

---

### 구조적 메모 (선택)
-BP에 바인딩을 맡기면 “동작은 되는데 가끔 안 되는” 초기화 순서 버그가 생기기 쉬움
    -> 핵심 연결부(위젯 생성/바인딩)는 C++에서 확정하는 편이 안전
- Weapon과 UI는 델리게이트로 연결하되, 바인딩 주체는 C++로 고정해 예측 가능성 확보

---

### ▶ 내일 할 일 (최대 3개)
- 재장전 인디케이터 UI 설계 (괄호형 게이지)
- Reload 진행도 계산 방식 결정 (시간 기반)
- 장전 시작/종료 이벤트를 UI 표시/숨김과 연결

---

---

## 📅 YYYY-MM-DD

### 🎯 오늘 목표 (최대 3개)
- 
- 
- 

---

### 완료한 작업
- 
- 
- 

---

### 발생한 문제
- 

---

### 해결 방법 / 결정 사항
- 

---

### 미완료 / 보류
- 

---

### 구조적 메모 (선택)
-
- 

---

### ▶ 내일 할 일 (최대 3개)
- 
- 
- 

---
