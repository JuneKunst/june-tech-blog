# Flutter SDUI 동적 위젯 팩토리 구현

> JSON 설정을 Flutter Widget으로 변환하는 핵심 구현

---

## 🔧 핵심 구현: 동적 위젯 팩토리

가장 중요한 부분은 **JSON 설정을 실제 Flutter Widget으로 변환**하는 팩토리입니다:

```dart
Widget _buildRightWidget({
  required ItemCategoryL1UiComponentConfig componentConfig,
  required CharanThemeData theme,
}) {
  // 🎯 핵심: JSON 타입에 따른 동적 위젯 생성
  switch (componentConfig.type) {
    case ItemCategoryL1UiComponentType.textButton:
      final textButtonConfig = componentConfig as ItemCategoryL1UiTextButtonConfig;
      return _buildTextButton(textButtonConfig: textButtonConfig, theme: theme);
      
    case ItemCategoryL1UiComponentType.gridThumbnailButtons:
      final gridConfig = componentConfig as ItemCategoryL1UiGridThumbnailButtonsConfig;
      return _buildGridThumbnailButtons(gridThumbnailButtonsConfig: gridConfig, theme: theme);
      
    case ItemCategoryL1UiComponentType.divider:
      return const Padding(
        padding: EdgeInsets.symmetric(horizontal: 4, vertical: 24),
        child: FlexDivider(),
      );
      
    default:
      return const SizedBox.shrink();
  }
}
```

## 🛡️ 타입 안전성 보장

복잡한 JSON 구조를 타입 안전하게 파싱:

```dart
List<ItemCategoryL1UiComponentConfig> _componentsFromJson(List<dynamic> componentJsons) {
  final componentConfigs = <ItemCategoryL1UiComponentConfig>[];
  
  for (final componentJson in componentJsons) {
    if (componentJson is! Map<String, dynamic>) continue;
    
    final baseConfig = ItemCategoryL1UiComponentConfig.fromJson(componentJson);
    
    // 🎯 타입별 구체적인 설정 객체 생성
    switch (baseConfig.type) {
      case ItemCategoryL1UiComponentType.menuButton:
        componentConfigs.add(ItemCategoryL1UiMenuButtonConfig.fromJson(componentJson));
        break;
      case ItemCategoryL1UiComponentType.textButton:
        componentConfigs.add(ItemCategoryL1UiTextButtonConfig.fromJson(componentJson));
        break;
    }
  }
  
  return componentConfigs;
}
```

## ⚠️ 핵심 교훈

**잘못된 예시** - 런타임 에러 위험:
```dart
Widget buildWidget(dynamic config) {
  return switch (config['type']) {
    'button' => ElevatedButton(onPressed: null, child: Text(config['text'])),
    _ => Container(), // 💥 런타임 에러 가능
  };
}
```

**올바른 예시** - 컴파일 타임 타입 체크:
```dart
Widget _buildWidget(ItemCategoryL1UiComponentConfig componentConfig) {
  switch (componentConfig.type) {
    case ItemCategoryL1UiComponentType.textButton:
      final textButtonConfig = componentConfig as ItemCategoryL1UiTextButtonConfig;
      return _buildTextButton(textButtonConfig: textButtonConfig);
    // ✅ 컴파일 타임에 모든 타입 보장
  }
}
```

---
**Tags**: `#flutter` `#server-driven-ui` `#dynamic-widget` `#type-safety`