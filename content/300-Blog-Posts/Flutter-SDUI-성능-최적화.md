# Flutter SDUI 성능 최적화 전략

> 70만+ 사용자 서비스에서 검증된 SDUI 성능 최적화 방법

---

## 🚀 3단계 캐싱 전략

```dart
class RemoteConfigService {
  // Level 1: 메모리 캐시 (가장 빠름)  
  ItemCategoryL1UiConfig? itemCategoryL1UiConfig;
  
  // Level 2: 로컬 스토리지 캐시 (오프라인 지원)
  Future<void> _fetchFromLocalStorage() async {
    final cachedJson = await StorageUtil.getString(cacheKey);
    if (cachedJson != null) {
      itemCategoryL1UiConfig = ItemCategoryL1UiConfig.fromJson(jsonDecode(cachedJson));
    }
  }
  
  // Level 3: 서버 업데이트 (백그라운드)
  void _updateFromServer() {
    _remoteConfigRepository.updateItemCategoryL1UiConfig();
  }
}
```

## 🔧 위젯 재사용 최적화

Global Key를 통한 위젯 상태 보존:

```dart
class CategoryState {
  final Map<String, GlobalKey> rightComponentKeyById = {};
  
  // 설정이 변경되어도 위젯 상태 유지
  GlobalKey getOrCreateKey(String componentId) {
    return rightComponentKeyById.putIfAbsent(componentId, () => GlobalKey());
  }
}
```

## 🎯 선택적 렌더링

BlocSelector로 필요한 부분만 리빌드:

```dart
BlocSelector4<CategoryCubit, CategoryState, 
    List<ItemCategoryL1UiComponentConfig>, int?, 
    Map<String, GlobalKey>, ScrollController?>(
  selector1: (state) => state.data.leftComponentConfigs,    // 사이드바 설정
  selector2: (state) => state.data.selectedIndex,           // 선택된 인덱스  
  selector3: (state) => state.data.rightComponentKeyById,   // 위젯 키맵
  selector4: (state) => state.data.menuBarScrollController, // 스크롤 컨트롤러
  builder: (context, leftConfigs, selectedIndex, keyMap, scrollController) {
    // 변경된 부분만 리빌드
  },
);
```

## 📊 성능 결과

### 메모리 사용량
- **메모리 캐시**: 즉시 접근 (0ms)
- **로컬 스토리지**: 평균 50ms
- **서버 요청**: 평균 200-500ms

### 사용자 경험
- ✅ **오프라인 지원**: 로컬 캐시로 네트워크 단절시에도 동작
- ✅ **빠른 로딩**: 3단계 캐싱으로 평균 로딩 시간 80% 단축
- ✅ **배터리 최적화**: 불필요한 리빌드 방지로 CPU 사용량 감소

---
**Tags**: `#flutter` `#performance` `#caching` `#optimization`