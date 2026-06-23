# Flashlight System

## Overview
어두운 환경에서 플레이어가 공간과 대상을 식별할 수 있도록 실제 빛을 제공하는 조명 시스템이다.
Flashlight는 시야 자체를 생성하지 않으며, `UTDVisionComponent`가 허용한 공간이 얼마나 밝게 보이는지를 바꾼다.
플레이어의 식별 능력을 높이는 대신 적에게 발각될 위험이 증가하는 Risk / Reward 구조를 가진다.

## Key Decisions
- Spatial Visibility와 Illumination을 분리한다.
- `UTDVisionComponent`는 시야 형상과 장애물 LOS만 판정한다.
- Flashlight는 실제 `SpotLight`를 사용하여 SceneColor, Mesh, 벽에 조명과 그림자를 적용한다.
- Flashlight가 켜져 있어도 벽 뒤나 Vision 범위 밖의 동적 대상은 표시하지 않는다.
- Vision 범위 안이라고 화면을 강제로 밝게 보정하지 않는다. Flashlight가 꺼진 어두운 공간은 계속 어둡게 보인다.
- 초기 구현에서는 조도 임계값을 Actor 표시·숨김 조건에 포함하지 않는다. 손전등 경계에서 대상이 팝인되는 현상을 피한다.
- Flashlight 사용 시 적의 탐지 속도 또는 Alert 보정값이 증가한다.
- 배터리 소모 자원 시스템은 이후 독립 작업 단위로 적용한다.

## Architecture
- Flashlight ON → `SpotLight` 활성화, 조명 범위 안의 실제 밝기 증가
- Flashlight OFF → `SpotLight` 비활성화, 환경의 기본 조명만 유지
- Vision System → Flashlight 상태와 무관하게 Spatial Visibility 판정 유지
- VisionRenderer → 시야 밖을 어둡게만 하며, 시야 안의 SceneColor와 Flashlight 조명 결과는 그대로 유지
- Actor Visibility System → `InVisionShape && HasLineOfSight` 결과로 동적 대상 표시·숨김
- Enemy Alert System → Flashlight 상태 또는 광원 노출을 별도 탐지 보정 입력으로 사용

```text
UTDVisionComponent
 └─ InVisionShape + HasLineOfSight
          ↓
UTDActorVisibilityComponent
 └─ 동적 대상 표시·숨김

SpotLight / World Lighting
 └─ 표시된 공간과 대상의 실제 밝기

Recognition  [추후 필요 시]
 └─ Spatial Visibility + Illumination
    └─ UI, 조준, 상호작용 등의 식별 보정
```

- 손전등 조명 거리와 `ConeVisionDistance`는 초기 튜닝에서 비슷하게 맞출 수 있지만, 서로 독립된 값과 책임으로 유지한다.
- 손전등에 따른 Vision 거리·각도 증가는 기본 책임이 아니라 선택적 Gameplay Modifier로만 검토한다.

## Trade-offs
- 실제 `SpotLight`를 사용하면 환경 조명과 자연스럽게 합성되지만 Dynamic Shadow 비용이 발생한다.
- 조도를 CPU에서 정확히 샘플링해 Boolean 판정에 사용하면 복잡도와 비용이 커진다. 초기 구현은 실제 렌더링 결과에 맡긴다.
- 얇은 벽, 그림자 비활성 Mesh, 부정확한 Collision 설정은 빛 또는 시야가 벽을 통과하는 불일치를 만들 수 있다.
- Auto Exposure가 손전등을 끈 공간까지 밝게 만들지 않도록 노출 설정을 제한할 필요가 있다.
- 손전등 광원 방향과 캐릭터의 Vision 방향이 어긋나면 밝지만 Actor가 숨거나, 어두운데 Actor가 표시되는 상황이 생길 수 있다.

## Future
- Flashlight 깜빡임
- AI가 Flashlight 직접 인식
- 조도 기반 Recognition 및 UI 표시 보정
- 선택적 Vision 거리·각도 Modifier
- 배터리 소모와 충전
- 관련 문서: [[vision-system]], [[enemy-alert-system]], [[aim-modifier-system]]
