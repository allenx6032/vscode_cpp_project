// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_CSARMATURENODE_FLATBUFFERS_H_
#define FLATBUFFERS_GENERATED_CSARMATURENODE_FLATBUFFERS_H_

#include "flatbuffers.h"


namespace flatbuffers {

struct CSArmatureNodeOption;
struct ResourceItemData;


struct CSArmatureNodeOption : private flatbuffers::Table {
	const WidgetOptions *nodeOptions() const { return GetPointer<const WidgetOptions *>(4); }
	const ResourceItemData *fileData() const { return GetPointer<const ResourceItemData *>(6); }
	uint8_t isLoop() const { return GetField<uint8_t>(8, 1); }
	uint8_t isAutoPlay() const { return GetField<uint8_t>(10, 1); }
	const flatbuffers::String *currentAnimationName() const { return GetPointer<const flatbuffers::String *>(12); }
	bool Verify(flatbuffers::Verifier &verifier) const {
		return VerifyTableStart(verifier) &&
			VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* nodeOptions */) &&
			verifier.VerifyTable(nodeOptions()) &&
			VerifyField<flatbuffers::uoffset_t>(verifier, 6 /* fileData */) &&
			verifier.VerifyTable(fileData()) &&
			VerifyField<uint8_t>(verifier, 8 /* isLoop */) &&
			VerifyField<uint8_t>(verifier, 10 /* isAutoPlay */) &&
			VerifyField<flatbuffers::uoffset_t>(verifier, 12 /* currentAnimationName */) &&
			verifier.Verify(currentAnimationName()) &&
			verifier.EndTable();
	}
};

struct CSArmatureNodeOptionBuilder {
	flatbuffers::FlatBufferBuilder &fbb_;
	flatbuffers::uoffset_t start_;
	void add_nodeOptions(flatbuffers::Offset<WidgetOptions> nodeOptions) { fbb_.AddOffset(4, nodeOptions); }
	void add_fileData(flatbuffers::Offset<ResourceItemData> fileData) { fbb_.AddOffset(6, fileData); }
	void add_isLoop(uint8_t isLoop) { fbb_.AddElement<uint8_t>(8, isLoop, 1); }
	void add_isAutoPlay(uint8_t isAutoPlay) { fbb_.AddElement<uint8_t>(10, isAutoPlay, 1); }
	void add_currentAnimationName(flatbuffers::Offset<flatbuffers::String> currentAnimationName) { fbb_.AddOffset(12, currentAnimationName); }
	CSArmatureNodeOptionBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
	CSArmatureNodeOptionBuilder &operator=(const CSArmatureNodeOptionBuilder &);
	flatbuffers::Offset<CSArmatureNodeOption> Finish() {
		auto o = flatbuffers::Offset<CSArmatureNodeOption>(fbb_.EndTable(start_, 5));
		return o;
	}
};

inline flatbuffers::Offset<CSArmatureNodeOption> CreateCSArmatureNodeOption(flatbuffers::FlatBufferBuilder &_fbb,
	flatbuffers::Offset<WidgetOptions> nodeOptions = 0,
	flatbuffers::Offset<ResourceItemData> fileData = 0,
	uint8_t isLoop = 1,
	uint8_t isAutoPlay = 1,
	flatbuffers::Offset<flatbuffers::String> currentAnimationName = 0) {
	CSArmatureNodeOptionBuilder builder_(_fbb);
	builder_.add_currentAnimationName(currentAnimationName);
	builder_.add_fileData(fileData);
	builder_.add_nodeOptions(nodeOptions);
	builder_.add_isAutoPlay(isAutoPlay);
	builder_.add_isLoop(isLoop);
	return builder_.Finish();
}

struct ResourceItemData : private flatbuffers::Table {
  int32_t type() const { return GetField<int32_t>(4, 0); }
  const flatbuffers::String *path() const { return GetPointer<const flatbuffers::String *>(6); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, 4 /* type */) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 6 /* path */) &&
           verifier.Verify(path()) &&
           verifier.EndTable();
  }
};

struct ResourceItemDataBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_type(int32_t type) { fbb_.AddElement<int32_t>(4, type, 0); }
  void add_path(flatbuffers::Offset<flatbuffers::String> path) { fbb_.AddOffset(6, path); }
  ResourceItemDataBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  ResourceItemDataBuilder &operator=(const ResourceItemDataBuilder &);
  flatbuffers::Offset<ResourceItemData> Finish() {
    auto o = flatbuffers::Offset<ResourceItemData>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<ResourceItemData> CreateResourceItemData(flatbuffers::FlatBufferBuilder &_fbb,
   int32_t type = 0,
   flatbuffers::Offset<flatbuffers::String> path = 0) {
  ResourceItemDataBuilder builder_(_fbb);
  builder_.add_path(path);
  builder_.add_type(type);
  return builder_.Finish();
}

inline const CSArmatureNodeOption *GetCSArmatureNodeOption(const void *buf) { return flatbuffers::GetRoot<CSArmatureNodeOption>(buf); }

inline bool VerifyCSArmatureNodeOptionBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<CSArmatureNodeOption>(); }

inline void FinishCSArmatureNodeOptionBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<CSArmatureNodeOption> root) { fbb.Finish(root); }

}  // namespace flatbuffers

#endif  // FLATBUFFERS_GENERATED_CSARMATURENODE_FLATBUFFERS_H_
