0
\# URDF (Unified Robot Description Format) 문법 정리 — ROS 2 기준

URDF는 XML 기반으로 로봇의 링크(부품)와 조인트(관절)를 트리 구조로 기술합니다. ROS 2에서는 순수 URDF보다 **xacro**를 섞어 쓰는 게 사실상 표준입니다.

## 1. 최상위 구조

```xml
<?xml version="1.0"?>
<robot name="my_robot" xmlns:xacro="http://www.ros.org/wiki/xacro">
  <link name="..."/>
  <joint name="..." type="...">...</joint>
</robot>
```

- 루트 태그는 `<robot>`, `name` 속성 필수.
- 내부에 `<link>`와 `<joint>`를 여러 개 나열해 트리를 구성 (링크=노드, 조인트=엣지).
- 트리이므로 **부모 없는 최상위 링크(base_link)가 하나** 있어야 하고 순환 구조는 불가.

## 2. `<link>` — 물리적 부품

```xml
<link name="wheel_left">
  <visual>
    <origin xyz="0 0 0" rpy="0 0 0"/>
    <geometry>
      <cylinder radius="0.05" length="0.02"/>
      <!-- box size="x y z" / sphere radius / mesh filename="package://.../a.dae" -->
    </geometry>
    <material name="black">
      <color rgba="0 0 0 1"/>
    </material>
  </visual>

  <collision>
    <origin xyz="0 0 0" rpy="0 0 0"/>
    <geometry><cylinder radius="0.05" length="0.02"/></geometry>
  </collision>

  <inertial>
    <origin xyz="0 0 0" rpy="0 0 0"/>
    <mass value="0.5"/>
    <inertia ixx="0.001" ixy="0" ixz="0" iyy="0.001" iyz="0" izz="0.001"/>
  </inertial>
</link>
```

| 태그          | 역할                                                                               |
| ------------- | ---------------------------------------------------------------------------------- |
| `visual`    | RViz 등에서 렌더링될 형상 (mesh 등 복잡한 모델 사용 가능)                          |
| `collision` | 충돌 계산용 형상 (보통 visual보다 단순화, 성능↑)                                  |
| `inertial`  | 질량·관성텐서 —**Gazebo/물리 시뮬레이션에 필수**, RViz만 쓸 거면 생략 가능 |

geometry 종류: `box`, `cylinder`, `sphere`, `mesh`(`.stl`, `.dae` 등, `package://` 경로 사용).

## 3. `<joint>` — 링크 간 연결

```xml
<joint name="wheel_left_joint" type="continuous">
  <parent link="base_link"/>
  <child link="wheel_left"/>
  <origin xyz="0 0.1 0" rpy="0 0 0"/>
  <axis xyz="0 1 0"/>
  <limit lower="-1.57" upper="1.57" effort="10" velocity="1.0"/>
  <dynamics damping="0.1" friction="0.0"/>
</joint>
```

**joint type** (필수):

| type           | 설명                                          |
| -------------- | --------------------------------------------- |
| `fixed`      | 고정, 움직이지 않음 (센서 마운트 등)          |
| `revolute`   | 회전, 각도 제한 있음 (관절 로봇 팔)           |
| `continuous` | 회전, 제한 없음 (바퀴)                        |
| `prismatic`  | 직선 이동, 위치 제한 있음 (리니어 액추에이터) |
| `floating`   | 6DOF 자유 이동 (거의 안 씀)                   |
| `planar`     | 평면상 이동                                   |

- `origin`: child가 parent 좌표계 기준으로 어디에 붙는지 (xyz + rpy, 라디안)
- `axis`: revolute/continuous/prismatic에서 움직이는 축
- `limit`: revolute/prismatic에 필수 (`lower`, `upper`, `effort`, `velocity`)

## 4. ROS 2에서 흔히 같이 쓰는 확장

### xacro (매크로/변수)

```xml
<xacro:property name="wheel_radius" value="0.05"/>

<xacro:macro name="wheel" params="prefix reflect">
  <link name="${prefix}_wheel">
    ...
    <cylinder radius="${wheel_radius}" length="0.02"/>
  </link>
</xacro:macro>

<xacro:wheel prefix="left" reflect="1"/>
<xacro:wheel prefix="right" reflect="-1"/>

<xacro:include filename="$(find my_pkg)/urdf/sensors.xacro"/>
```

- `.urdf.xacro` 파일로 작성 후 `xacro robot.urdf.xacro > robot.urdf` 또는 launch 파일에서 `xacro.process_file()`로 변환해서 사용.
- 반복 구조(바퀴 4개, 손가락 등)를 매크로로 처리할 때 필수.

### `<ros2_control>` — 하드웨어 인터페이스 (ROS 2 전용)

```xml
<ros2_control name="MyRobotSystem" type="system">
  <hardware>
    <plugin>gazebo_ros2_control/GazeboSystem</plugin>
    <!-- 또는 실기체용 커스텀 hardware_interface 플러그인 -->
  </hardware>
  <joint name="wheel_left_joint">
    <command_interface name="velocity"/>
    <state_interface name="position"/>
    <state_interface name="velocity"/>
  </joint>
</ros2_control>
```

- `ros2_control`이 어떤 조인트를 command/state 인터페이스로 제어할지 선언.
- `hardware/plugin`에는 시뮬레이션용(`gazebo_ros2_control/GazeboSystem`) 또는 실제 하드웨어 드라이버 플러그인 지정.
- `command_interface`: 컨트롤러가 내려보내는 값 (position/velocity/effort).
- `state_interface`: 컨트롤러가 읽어오는 값 (position/velocity/effort).
- 실제 제어 알고리즘(예: `diff_drive_controller`, `joint_trajectory_controller`)은 별도 `controllers.yaml`에서 설정하고 `ros2_control_node`가 로드.

### `<gazebo>` 태그 (시뮬레이션 전용 설정)

```xml
<gazebo reference="wheel_left">
  <mu1>1.0</mu1>
  <mu2>1.0</mu2>
</gazebo>

<gazebo>
  <plugin filename="libgazebo_ros2_control.so" name="gazebo_ros2_control">
    <parameters>$(find my_pkg)/config/controllers.yaml</parameters>
  </plugin>
</gazebo>
```

- `reference` 속성으로 특정 링크에 마찰(`mu1`/`mu2`), 색상 등 Gazebo 전용 물리 속성 부여.
- `reference` 없이 쓰면 로봇 전체에 적용되는 플러그인 선언(예: `gazebo_ros2_control` 플러그인 로딩).

## 5. 일반적인 사용 흐름 (ROS 2)

1. `urdf/robot.urdf.xacro` 작성 (link, joint, xacro 매크로 포함)
2. `ros2_control` 태그로 제어 인터페이스 선언
3. `robot_state_publisher`가 URDF를 읽어 `/tf`, `/robot_description` 퍼블리시
4. `joint_state_publisher`(또는 실제 조인트 상태) → RViz에서 시각화
5. Gazebo 시뮬레이션 시 `<gazebo>` 플러그인이 `controllers.yaml`을 로드해 `ros2_control_node` 구동

## 6. 자주 하는 실수

- `inertial` 값이 0이거나 비현실적이면 Gazebo에서 로봇이 날아가거나 뒤집힘.
- `collision` 형상을 mesh 그대로 쓰면 물리 연산이 무겁고 불안정 → 단순 도형으로 근사 권장.
- `axis`의 방향과 `origin`의 `rpy` 좌표계를 혼동하기 쉬움 (axis는 child 링크 좌표계 기준).
- xacro `${}` 수식 안에서는 단위가 자동 변환되지 않으므로 항상 SI 단위(m, rad, kg) 통일 필요.
