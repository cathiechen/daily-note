# The process to add a featrue in blink.

- Patches is not the only thing required.
- Add feature flags, `https://cs.chromium.org/chromium/src/docs/how_to_add_your_feature_flag.md`
  - example https://chromium-review.googlesource.com/c/chromium/src/+/1349476
  - enums.xml could be update by `./out/Default/unit_tests --gtest_filter=AboutFlagsHistogramTest.CheckHistograms`
  - `./out/Default/unit_tests --gtest_filter=AboutFlagsTest.EveryFlagHasMetadata` will check `chrome/browser/flag-metadata.json`
- intent-to-ship
  - Explainer
  - useCounter to check the risk: https://chromium-review.googlesource.com/c/chromium/src/+/1741776, still `./out/Default/unit_tests --gtest_filter=AboutFlagsHistogramTest.CheckHistograms` will generate the change in enums.xml. useCounter is based on 
  - useCounter might could look in here: https://www.chromestatus.com/metrics/css/popularity, https://www.chromestatus.com/metrics/feature/timeline/popularity/328
  - add chromestatus entry: https://www.chromestatus.com/, to new a chromiumstatus: https://www.chromestatus.com/admin/features/new, example: https://www.chromestatus.com/feature/5759578031521792
  - and  Finch flag ?
  - And many todos:)

