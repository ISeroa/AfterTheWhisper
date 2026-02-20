# Ammo Indicator UI
## Overview

무기 탄약 상태를 시각적으로 표현하는 Ammo Indicator UI를 구현한다.

현재 탄약 / 최대 탄약 표시

탄창 크기에 따라 분할되는 원형 파이 UI

마우스를 따라다니는 HUD

이벤트 기반 상태 갱신

Architecture
WeaponBase

OnAmmoChanged 델리게이트 브로드캐스트

탄약 변경 시 이벤트 발생

UI를 직접 참조하지 않음

Ammo Widget

Weapon 이벤트 구독

캐시 기반 중복 업데이트 방지

NativeTick으로 마우스 위치 추적

Dynamic Material로 파이 표현

Update Flow
Fire / Reload
→ AmmoInMag 변경
→ OnAmmoChanged.Broadcast()
→ Widget 갱신