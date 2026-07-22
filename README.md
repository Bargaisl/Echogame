# ECHO — Magic Survival 3D Horde

> **Version:** Jam Build  
> **File size:** ~608 KB (compressed) | Limit: 1 474 560 bytes  
> **Engine:** Custom C99 + Raylib 6.0  
> **Platform:** Windows 64-bit (standalone executable)

---

## STORY

You are a lone mage stranded in the Void — an infinite dark cosmic plane suspended between dimensions. Below you: a black hole singularity. Above: a starless abyss. Waves of shadow creatures pour from the dark. You must survive, level up your spells, and reach the Portal of Silence to escape.

---

## HOW TO PLAY

### Movement & Camera
| Key / Input | Action |
|---|---|
| `W A S D` | Move the mage |
| `Mouse` | Look / rotate camera |
| `Left Shift` | Ghost Mode — phase through enemies (costs Ghost charge) |
| `Left Mouse Button (LMB)` | Echo Pulse — echolocation wave that reveals objects & pushes enemies |
| `Right Mouse Button (RMB)` | Deploy Decoy — drops a glowing lure that attracts enemies |
| `G` | Open Guide & Abilities reference screen |
| `S` | Open Settings & Language menu |
| `ESC` | Back / Pause |
| `Space / Click` | Start game from main menu |
| `+` / `-` | Increase / Decrease master volume |
| `1 2 3` | Select upgrade card during level-up |

---

## WIN CONDITION

Collect **all Echo Crystals** scattered across the arena, then reach the **Portal of Silence** to escape the Void.

---

## PICKUPS & COLLECTIBLES

| Object | Color | Effect |
|---|---|---|
| **Blue Mana Orb** | Cyan | +15 Mana XP |
| **Yellow Mana Orb** | Gold | +50 Mana XP |
| **Heart** | Red/Pink | +25 HP (restores health) |
| **Echo Crystal** | Gold/Cyan | Counts toward win condition |
| **Silence Node** | Teal | +45 Ghost charge |
| **Treasure Chest** | Gold | Rare artifact unlock (walk into it) |
| **Portal of Silence** | Magenta | WIN — escape the Void |

> Hearts also drop from defeated enemies (~20% chance).

---

## UPGRADES — LEVEL UP CARDS

Every time your Mana XP bar fills, you level up and choose 1 of 3 random upgrades:

| Upgrade | Type | Effect |
|---|---|---|
| **Magic Bolt** | Offensive | Homing plasma projectiles, fast fire rate |
| **Fireball** | Offensive | Explosive AoE fire sphere |
| **Thunderstorm** | Offensive | Sky lightning strikes on priority targets |
| **Meteor** | Offensive | Flaming meteor impact, huge crater damage |
| **Satellites** | Offensive | Orbiting contact spheres — continuous damage |
| **Arcane Ray** | Offensive | Piercing laser beam through all enemies |
| **Lava Zone** | Offensive | Burning magma pools on the ground |
| **Tsunami** | Offensive | Giant tidal wave clearing the arena |
| **Electric Zone** | Offensive | High-voltage shock ring aura |
| **Shield** | Utility | Energy dome absorbing direct hits |
| **Armageddon** | Special | Screen-clearing explosion — destroys entire horde |
| **Magic Circle** | Utility | Ground power ring: +25% ATK & cast speed |
| **Intelligence** | Passive | +20% all spell damage |
| **Fast Casting** | Passive | −25% cooldown for all spells |
| **Haste & Explorer** | Passive | +25% move speed + item magnet radius |
| **Arcana Mastery** | Special | +30% spell size & critical hit chance |

---

## RARE ARTIFACTS (from Treasure Chests)

| Artifact | Effect |
|---|---|
| **Boots of Hermes** | +30% movement speed |
| **Amulet of Vitality** | +50 max HP & full heal |
| **Chronos Hourglass** | −35% spell cooldown |
| **Orb of Reflection** | +35% all spell damage |
| **Heart of Phoenix** | Passive HP regeneration (+3 HP/sec) |

---

## ENEMIES

| Type | Appearance | Behavior |
|---|---|---|
| **Tentacle Imp** | Black sphere with pink tentacles | Swarms from all directions |
| **Red Beast** | Red sphere with cyan eyes & tentacles | Faster, tougher |
| **Black Hole Wraith** | Black core with magenta rings & energy beams | Elite — high HP, dangerous aura |

> Enemy count scales over time. Enemies primarily spawn in front of you (forward spawning).

---

## HUD — ON-SCREEN INTERFACE

| Element | Location | Description |
|---|---|---|
| Health bar | Top-left | Red/pink fill — current / max HP |
| Mana XP bar | Top-left | Gold fill — XP toward next level |
| Ghost bar | Top-left | Teal fill — Ghost mode charge |
| Echo Pulse bar | Top-left | Cyan fill — cooldown readiness |
| Timer & Kills | Top-center | Survival time + kill count |
| Radar | Top-right | Mini-map showing nearby objects & enemies |
| Crosshair | Screen center | Aim point |

### Radar Color Legend
- 🔵 Cyan dot = Blue mana orb  
- 🟡 Gold dot = Yellow mana orb  
- 🟥 Gold square = Treasure chest  
- 🔴 Pink dot = Heart pickup  
- 🟣 Magenta dot = Portal of Silence  
- 🔴 Red dot = Elite enemy  
- 🩷 Pink dot = Normal enemy  

---

## SETTINGS

- **Language:** English / Russian / Korean (South)
- **Master Volume:** +/- keys or settings menu

---

## LANGUAGES

- English (EN)
- Русский (RU)
- 한국어 (KR)

---
---

# ECHO — 마법 생존 3D 호드

> **버전:** Jam 빌드  
> **파일 크기:** ~608 KB (압축) | 제한: 1 474 560 바이트  
> **엔진:** 커스텀 C99 + Raylib 6.0  
> **플랫폼:** Windows 64비트 (단독 실행 파일)

---

## 스토리

당신은 차원 사이의 무한한 어둠의 공간 — 공허(Void) — 에 홀로 남겨진 마법사입니다. 발 아래에는 블랙홀 특이점이 있고, 위에는 별도 없는 심연이 있습니다. 어둠의 생명체들이 쏟아지고 있습니다. 살아남고, 주문을 강화하고, 침묵의 포탈을 통해 탈출하세요.

---

## 게임 방법

### 이동 및 카메라
| 키 / 입력 | 행동 |
|---|---|
| `W A S D` | 마법사 이동 |
| `마우스` | 시점 / 카메라 회전 |
| `Left Shift` | 유령 모드 — 적을 통과 (유령 게이지 소모) |
| `마우스 왼쪽 버튼` | 에코 펄스 — 물체 탐지 및 적 밀치기 |
| `마우스 오른쪽 버튼` | 미끼 배치 — 적을 유인하는 미끼 투하 |
| `G` | 가이드 & 능력 화면 열기 |
| `S` | 설정 & 언어 메뉴 열기 |
| `ESC` | 뒤로 / 일시정지 |
| `스페이스 / 클릭` | 메인 메뉴에서 게임 시작 |
| `+` / `-` | 마스터 볼륨 조절 |
| `1 2 3` | 레벨업 중 업그레이드 카드 선택 |

---

## 승리 조건

경기장에 흩어진 **모든 에코 크리스탈**을 모은 후 **침묵의 포탈**에 도달하여 공허에서 탈출하세요.

---

## 픽업 및 수집품

| 물체 | 색상 | 효과 |
|---|---|---|
| **파란 마나 오브** | 청록색 | 마나 XP +15 |
| **노란 마나 오브** | 금색 | 마나 XP +50 |
| **하트** | 빨강/분홍 | HP +25 (체력 회복) |
| **에코 크리스탈** | 금색/청록 | 승리 조건 카운트 |
| **침묵 노드** | 청록색 | 유령 게이지 +45 |
| **보물 상자** | 금색 | 희귀 아티팩트 획득 |
| **침묵의 포탈** | 자홍색 | 승리 — 공허 탈출 |

---

## 업그레이드 — 레벨업 카드

마나 XP 바가 가득 차면 레벨업되고 3개의 랜덤 업그레이드 중 1개를 선택합니다:

| 업그레이드 | 유형 | 효과 |
|---|---|---|
| **마법 화살** | 공격형 | 유도 플라즈마 발사체, 빠른 발사 속도 |
| **파이어볼** | 공격형 | 폭발적인 화염구 AoE |
| **뇌우** | 공격형 | 우선 목표에 번개 공격 |
| **운석** | 공격형 | 불타는 운석 충격, 거대 피해 |
| **위성** | 공격형 | 궤도 접촉 구체 — 지속 피해 |
| **비전 광선** | 공격형 | 모든 적을 관통하는 레이저 빔 |
| **용암 지대** | 공격형 | 지면의 불타는 용암 웅덩이 |
| **쓰나미** | 공격형 | 경기장을 청소하는 거대한 파도 |
| **전기 지대** | 공격형 | 고압 충격 링 오라 |
| **방어막** | 유틸리티 | 직접 타격을 흡수하는 에너지 돔 |
| **아마겟돈** | 특수 | 화면 전체 폭발 — 전체 무리 파괴 |
| **마법 원** | 유틸리티 | 지면 파워 링: 공격력 +25% & 시전 속도 |
| **지능** | 패시브 | 모든 마법 피해 +20% |
| **빠른 시전** | 패시브 | 모든 주문 쿨다운 −25% |
| **신속 & 탐험가** | 패시브 | 이동 속도 +25% + 아이템 자석 반경 |
| **비전 마스터리** | 특수 | 마법 크기 +30% & 치명타 확률 |

---

## HUD — 화면 인터페이스

| 요소 | 위치 | 설명 |
|---|---|---|
| 체력 바 | 좌상단 | 빨강/분홍 — 현재/최대 HP |
| 마나 XP 바 | 좌상단 | 금색 — 다음 레벨까지의 XP |
| 유령 바 | 좌상단 | 청록색 — 유령 모드 게이지 |
| 에코 펄스 바 | 좌상단 | 청록색 — 쿨다운 준비 상태 |
| 타이머 & 킬 수 | 상단 중앙 | 생존 시간 + 킬 수 |
| 레이더 | 우상단 | 주변 물체 및 적 미니맵 |
| 조준점 | 화면 중앙 | 조준 지점 |

---
---

# ECHO — Магическое выживание 3D Орда

> **Версия:** Jam-билд  
> **Размер файла:** ~608 КБ (сжатый) | Лимит: 1 474 560 байт  
> **Движок:** Собственный C99 + Raylib 6.0  
> **Платформа:** Windows 64-бит (автономный исполняемый файл)

---

## ИСТОРИЯ

Ты — одинокий маг, заброшенный в Пустоту — бесконечную тёмную космическую плоскость между измерениями. Под ногами — сингулярность чёрной дыры. Над головой — беззвёздная бездна. Отовсюду наваливаются существа тьмы. Выживи, прокачай заклинания и добберись до Портала Тишины, чтобы сбежать.

---

## КАК ИГРАТЬ

### Управление и камера
| Клавиша / Ввод | Действие |
|---|---|
| `W A S D` | Движение мага |
| `Мышь` | Обзор / поворот камеры |
| `Left Shift` | Режим призрака — проход сквозь врагов (тратит заряд призрака) |
| `ЛКМ (левая кнопка мыши)` | Эхо-импульс — волна эхолокации, раскрывает объекты и отталкивает врагов |
| `ПКМ (правая кнопка мыши)` | Поставить приманку — сбрасывает светящуюся ловушку для врагов |
| `G` | Открыть гайд и справочник по способностям |
| `S` | Открыть настройки и язык |
| `ESC` | Назад / Пауза |
| `Пробел / Клик` | Начать игру из главного меню |
| `+` / `-` | Увеличить / уменьшить громкость |
| `1 2 3` | Выбрать карточку прокачки при повышении уровня |

---

## УСЛОВИЕ ПОБЕДЫ

Собери **все Эхо-Кристаллы**, разбросанные по арене, затем добберись до **Портала Тишины**, чтобы сбежать из Пустоты.

---

## ПОДБИРАЕМЫЕ ПРЕДМЕТЫ

| Предмет | Цвет | Эффект |
|---|---|---|
| **Синяя сфера маны** | Голубой | +15 Мана XP |
| **Жёлтая сфера маны** | Золотой | +50 Мана XP |
| **Сердечко** | Красный/Розовый | +25 HP (восстанавливает здоровье) |
| **Эхо-Кристалл** | Золотой/Голубой | Считается к условию победы |
| **Узел тишины** | Бирюзовый | +45 заряда призрака |
| **Сундук сокровищ** | Золотой | Разблокировка редкого артефакта |
| **Портал тишины** | Розово-малиновый | ПОБЕДА — побег из Пустоты |

> Сердечки также выпадают из убитых врагов (~20% шанс).

---

## ПРОКАЧКА — КАРТОЧКИ НАВЫКОВ

Каждый раз, когда шкала Мана XP заполняется, происходит повышение уровня и выбор 1 из 3 случайных улучшений:

| Навык | Тип | Эффект |
|---|---|---|
| **Магический болт** | Атака | Самонаводящиеся снаряды, высокая скорострельность |
| **Огненный шар** | Атака | Взрывной огненный шар с уроном по области |
| **Грозовой шторм** | Атака | Молнии с неба по сильнейшим врагам |
| **Метеорит** | Атака | Падение огненного метеора, огромный урон |
| **Спутники** | Атака | Вращающиеся сферы вокруг мага — постоянный урон |
| **Лазерный луч** | Атака | Пронизывающий луч через всех врагов на линии |
| **Зона лавы** | Атака | Огненные лужи лавы на земле |
| **Цунами** | Атака | Гигантская волна сносит всю арену |
| **Электро-поле** | Атака | Кольцо высоковольтных молний вокруг мага |
| **Защитный щит** | Защита | Энергетический купол поглощает удары |
| **Армагеддон** | Особое | Взрыв на весь экран — уничтожает всю орду |
| **Магический круг** | Усиление | Круг силы под ногами: +25% к урону и скорости заклинаний |
| **Интеллект** | Пассивное | +20% ко всему урону заклинаний |
| **Быстрое колдовство** | Пассивное | −25% перезарядка всех заклинаний |
| **Ускорение & Магнит** | Пассивное | +25% к скорости + притягивание предметов |
| **Мастер магии** | Особое | +30% к размеру заклинаний и критический урон |

---

## РЕДКИЕ АРТЕФАКТЫ (из сундуков)

| Артефакт | Эффект |
|---|---|
| **Сапоги Гермеса** | +30% к скорости передвижения |
| **Амулет жизненной силы** | +50 макс. HP и полное лечение |
| **Песочные часы Хроноса** | −35% перезарядка заклинаний |
| **Сфера отражения** | +35% к урону всех заклинаний |
| **Сердце Феникса** | Пассивная регенерация HP (+3 HP/сек) |

---

## ВРАГИ

| Тип | Внешность | Поведение |
|---|---|---|
| **Щупальцевый бес** | Чёрная сфера с розовыми щупальцами | Роится со всех направлений |
| **Красный мутант** | Красная сфера, голубые глаза | Быстрее, опаснее |
| **Призрак чёрной дыры** | Чёрное ядро с кольцами и лучами | Элита — много HP, опасная аура |

> Количество врагов растёт со временем. Враги преимущественно появляются спереди от игрока.

---

## HUD — ИНТЕРФЕЙС НА ЭКРАНЕ

| Элемент | Расположение | Описание |
|---|---|---|
| Шкала здоровья | Левый верх | Красный/розовый — текущее/максимальное HP |
| Шкала Мана XP | Левый верх | Золотой — XP до следующего уровня |
| Шкала призрака | Левый верх | Бирюзовый — заряд режима призрака |
| Шкала Эхо-импульса | Левый верх | Голубой — готовность к использованию |
| Таймер и убийства | Верхний центр | Время выживания + количество убийств |
| Радар | Правый верх | Мини-карта с объектами и врагами |
| Прицел | Центр экрана | Точка прицеливания |

### Легенда цветов радара
- 🔵 Голубая точка = Синяя сфера маны  
- 🟡 Золотая точка = Жёлтая сфера маны  
- 🟥 Золотой квадрат = Сундук  
- 🩷 Розовая точка = Сердечко (HP)  
- 🟣 Малиновая точка = Портал тишины  
- 🔴 Красная точка = Элитный враг  
- 🩷 Розовая точка = Обычный враг  

---

## НАСТРОЙКИ

- **Язык:** English / Русский / 한국어
- **Громкость:** клавиши +/− или меню настроек
