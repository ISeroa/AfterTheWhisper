# Inventory-Loot-System

## Overview

Inventory & Loot System은 플레이어가 맵에서 아이템을 획득하고 관리할 수
있도록 하는 핵심 시스템이다.

이 시스템은 DataAsset 기반 구조를 사용하여 아이템 데이터와 루트 확률을
분리하며, 상자, 적 드롭, 바닥 아이템을 하나의 구조로 통합한다.

------------------------------------------------------------------------

## Key Decisions

-   LootTable은 **상자를 처음 열 때 생성**한다
    → 맵 로드시 불필요한 연산을 방지

-   루트 확률은 **SpawnWeight 기반 가중치 랜덤**을 사용한다
    → 확률 조정이 쉽고 데이터 중심 설계에 적합

-   인벤토리는 **InventoryComponent**에서 관리한다
    → Player / Chest / LootBag에서 동일 로직 사용 가능

-   무게 시스템은 **WeightClass 기반**으로 구현한다
    → 초기 구현 단순화

-   아이템 스택은 기본적으로 허용하며\
    **StackMax = 1**인 아이템만 스택 불가 처리한다

------------------------------------------------------------------------

## Architecture

아이템 시스템 전체 구조:

    ItemDataAsset
        |
        v
    InventoryComponent
        |
        |-- PlayerCharacter
        |
        |-- ChestActor
        |
        |-- LootBagActor
                |
                v
            LootTableDataAsset

### ItemDataAsset

아이템의 기본 데이터를 정의하는 DataAsset.

주요 데이터:

    ItemID
    Icon
    StackMax
    WeightClass

WeightClass는 아이템 무게를 간단하게 표현하기 위한 값이다.

------------------------------------------------------------------------

### InventoryComponent

모든 인벤토리 로직은 InventoryComponent에서 처리된다.

다음 Actor들이 InventoryComponent를 가진다.

-   PlayerCharacter
-   ChestActor
-   LootBagActor

주요 기능:

-   아이템 추가
-   아이템 제거
-   스택 처리
-   총 무게 계산
-   무게 초과 검사

아이템 추가 규칙:

    TotalWeight + ItemWeight <= MaxCarryWeight

조건을 만족하지 못하면 아이템을 획득할 수 없다.

------------------------------------------------------------------------

### Item Stacking

아이템 스택 여부는 다음 규칙으로 결정된다.

    StackMax > 1  -> 스택 가능
    StackMax = 1  -> 스택 불가

예시:

    Bandage   StackMax = 5
    Ammo      StackMax = 60
    Weapon    StackMax = 1

------------------------------------------------------------------------

### LootTable System

LootTable DataAsset은 루팅 아이템 생성을 담당한다.

LootTable은 다음 데이터를 가진다.

    RollsMin
    RollsMax
    LootEntries

LootEntry 구조:

    Item
    SpawnWeight
    MinCount
    MaxCount

SpawnWeight는 아이템 등장 확률을 결정하는 가중치 값이다.

------------------------------------------------------------------------

### Loot Generation Algorithm

루트 생성은 Roll 기반 방식으로 이루어진다.

    RollCount = Random(RollsMin, RollsMax)

    for each roll
        Item  = WeightedRandom(SpawnWeight)
        Count = Random(MinCount, MaxCount)

        Inventory.AddItem(Item, Count)

특징:

-   중복 아이템 허용
-   각 상자마다 다른 결과 생성
-   확률 조정이 쉬움

------------------------------------------------------------------------

### Container Actors

루팅 가능한 컨테이너는 두 종류가 존재한다.

#### ChestActor

맵에 배치된 상자.

특징:

-   LootTable을 가진다
-   **처음 열 때 Loot 생성**
-   InventoryComponent를 사용하여 아이템 보관

------------------------------------------------------------------------

#### LootBagActor

적 사망 시 생성되는 루팅 가방.

생성 과정:

    Enemy Death
        ↓
    Ragdoll
        ↓
    Dissolve
        ↓
    LootBagActor Spawn

LootBag 특징:

-   LootTable 기반 아이템 생성
-   상자와 동일한 루팅 인터페이스 사용
-   InventoryComponent 사용

------------------------------------------------------------------------

#### PickupActor

바닥에 떨어진 아이템을 표현하는 Actor.

특징:

-   E 키 상호작용으로 즉시 획득
-   인벤토리 무게 초과 시 획득 불가
-   단일 아이템 표현

------------------------------------------------------------------------

## Trade-offs

-   WeightClass 기반 시스템은 구현이 단순하지만
    정밀한 무게 밸런싱에는 한계가 있다.

-   LootTable이 단일 구조이기 때문에
    아이템 수가 매우 많아질 경우 관리가 어려워질 수 있다.

-   지역 기반 루트 시스템은 현재 구현되어 있지 않다.

------------------------------------------------------------------------

## Future

-   float 기반 ItemWeight 시스템으로 확장
-   지역별 LootTable 분리
-   컨테이너 타입별 루트 확률 차등 적용
-   LootBag 자동 제거 규칙 추가
