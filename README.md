# AfterTheWisper

> Portfolio project developed with Unreal Engine 4.25 (C++)

A story-driven top-down shooter focused on clean architecture, data-driven systems, and scalable gameplay design.

---

## Overview

Project Name is a narrative-focused top-down shooter set in a collapsing world inspired by cosmic horror themes.

Players explore hazardous surface zones to retrieve resources and artifacts while preparing their underground base for incoming enemy assaults. As time progresses, enemies become stronger and survival requires strategic preparation.

The project prioritizes maintainable C++ architecture, low coupling between systems, and scalability for future feature expansion.

---

## Tech Stack

- Unreal Engine 4.25
- C++
- DataAsset-driven configuration system
- StaticMesh-based modular weapon assembly
- Projectile-based shooting system
- Git (Version Control)

---

## Core Systems

### Weapon System
- DataAsset-based weapon configuration
- Modular weapon structure (Frame + Parts)
- Expandable shooting logic for future features (penetration / ricochet)

### Shooting System
- Projectile-based firing
- Designed for extensibility (damage types, behaviors)

### Architecture Focus
- Clear class responsibility separation
- Low dependency coupling
- Buildable commit history
- Optimization-conscious design

---

## Project Structure

```
Source/
 ├─ Character/
 ├─ Weapon/
 ├─ Data/
 ├─ Systems/
```

(Content folder structured separately for assets and data)

---


## Future Plans

- Enemy AI implementation
- Base defense mechanics
- Save/Load system
- Expanded weapon types
- Narrative event system

---

## Notes

This project is a personal portfolio project.  
All third-party assets used are either Unreal default assets or legally distributed free assets.

---

---

# 🇰🇷 한국어 설명

## 프로젝트 개요

AfterTheWisper는 크툴루풍 세계관에서 진행되는 스토리 중심 탑다운 슈터입니다.

플레이어는 지상에서 자원과 유물을 수집하고, 지하 거점을 강화하며, 시간이 지날수록 강해지는 적들의 습격에 대비해야 합니다.

단순 생존 중심이 아닌 서사 중심 진행을 목표로 설계되었습니다.

---

## 기술 스택

- Unreal Engine 4.25
- C++
- DataAsset 기반 설정 시스템
- StaticMesh 기반 모듈형 무기 조립 구조
- Projectile 기반 발사 시스템
- Git 버전 관리

---

## 주요 시스템

### 무기 시스템
- DataAsset 기반 무기 데이터 관리
- Frame + Part 구조의 모듈형 조립
- 향후 관통 / 튕김 기능 확장을 고려한 구조

### 발사 시스템
- Projectile + linetrace 기반 처리
- 확장 가능한 데미지 구조 설계

### 아키텍처 설계 방향
- 명확한 클래스 역할 분리
- 낮은 결합도 유지
- 빌드 가능한 커밋 히스토리 관리
- 최적화를 고려한 설계

---

## 향후 계획

- 적 AI 구현
- 거점 방어 시스템
- 저장 / 로드 시스템
- 무기 확장
- 스토리 이벤트 시스템

---

본 프로젝트는 개인 포트폴리오 용도로 제작되었습니다.
사용된 외부 에셋은 Unreal 기본 에셋 또는 무료 배포 에셋입니다.
