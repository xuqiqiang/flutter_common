
Map<String, dynamic> filter(Map<String, dynamic> map) {
  var newMap = <String, dynamic>{};
  map.forEach((key, value) {
    if (value != null) newMap[key] = value;
  });
  return newMap;
}
