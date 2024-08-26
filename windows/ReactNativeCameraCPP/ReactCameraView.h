#pragma once

#include "JSValue.h"
#include "NativeModules.h"

#include "CameraRotationHelper.h"
#include "ReactCameraConstants.h"

#include "winrt/Microsoft.AI.Skills.SkillInterface.h"
#include "winrt/Microsoft.AI.Skills.Vision.ObjectDetector.h"

namespace winrt::ReactNativeCameraCPP {
struct ReactCameraView : winrt::Windows::UI::Xaml::Controls::GridT<ReactCameraView> {
 public:
  ReactCameraView() = default;
  ~ReactCameraView();
  void SetContext(winrt::Microsoft::ReactNative::IReactContext const &reactContext);
  void Initialize();
  void UpdateProperties(winrt::Microsoft::ReactNative::IJSValueReader const &propertyMapReader) noexcept;

  winrt::Windows::Foundation::IAsyncAction TakePictureAsync(
      winrt::Microsoft::ReactNative::JSValueObject const &options,
      winrt::Microsoft::ReactNative::ReactPromise<winrt::Microsoft::ReactNative::JSValueObject> const &result) noexcept;
  winrt::Windows::Foundation::IAsyncAction RecordAsync(
      winrt::Microsoft::ReactNative::JSValueObject const &options,
      winrt::Microsoft::ReactNative::ReactPromise<winrt::Microsoft::ReactNative::JSValueObject> const &result) noexcept;
  winrt::Windows::Foundation::IAsyncAction StopRecordAsync() noexcept;
  winrt::Windows::Foundation::IAsyncAction IsRecordingAsync(
      winrt::Microsoft::ReactNative::ReactPromise<bool> const &result) noexcept;
  winrt::Windows::Foundation::IAsyncAction PausePreviewAsync() noexcept;
  winrt::Windows::Foundation::IAsyncAction ResumePreviewAsync() noexcept;

 public:
  static winrt::com_ptr<ReactCameraView> Create();

 private:
  void UpdateKeepAwake(bool keepAwake);
  void UpdateFlashMode(int flashMode);
  void UpdateAutoFocus(int focusMode);
  void UpdateWhiteBalance(int whiteBalance);
  void UpdateMirrorVideo(bool mirrorVideo);
  void UpdateAspect(int aspect);
  void UpdateDefaultVideoQuality(int videoQuality);
  void UpdateBarcodeScannerEnabled(bool barcodeScannerEnabled);
  void UpdateFaceDetectEnabled(bool faceDetectEnabled);
  void UpdateObjectDetectEnabled(bool objectDetectEnabled);
  void UpdateBarcodeTypes(winrt::Microsoft::ReactNative::JSValueArray const &barcodeTypes);
  void UpdateBarcodeReadIntervalMS(int barcodeReadIntervalMS);

  fire_and_forget UpdateDeviceId(std::string cameraId);
  fire_and_forget UpdateDeviceType(int type);

  winrt::Windows::Foundation::IAsyncAction InitializeAsync();

  winrt::Windows::Foundation::IAsyncAction UpdateMediaStreamPropertiesAsync();
  winrt::Windows::Foundation::IAsyncAction UpdateMediaStreamPropertiesAsync(int videoQuality);

  winrt::Windows::Foundation::IAsyncAction CleanupMediaCaptureAsync();
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Devices::Enumeration::DeviceInformation>
  FindCameraDeviceAsync();
  winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> GetBase64DataAsync(
      winrt::Windows::Storage::Streams::IRandomAccessStream stream);
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> GetOutputStorageFileAsync(
      int type,
      int target);
  void DelayStopRecording(float totalRecordingInSecs);
  winrt::Windows::Foundation::IAsyncAction WaitAndStopRecording();
  winrt::Windows::Foundation::IAsyncAction UpdatePreviewOrientationAsync();
  winrt::Windows::Foundation::IAsyncAction UpdateFilePropertiesAsync(
      winrt::Windows::Storage::StorageFile storageFile,
      winrt::Microsoft::ReactNative::JSValueObject const &options);

  void StartBarcodeScanner();
  void StopBarcodeScanner();
  winrt::Windows::Foundation::IAsyncAction ScanForBarcodeAsync();
  void StartFaceDetect();
  void StopFaceDetect();
  winrt::Windows::Foundation::IAsyncAction ScanForFaceAsync();
  void StartObjectDetect();
  void StopObjectDetect();
  winrt::Windows::Foundation::IAsyncAction ScanForObjectAsync();

  void OnOrientationChanged(const bool updatePreview);
  void OnApplicationSuspending();
  void OnApplicationResuming();
  winrt::Windows::Foundation::IAsyncAction OnUnloaded();

  winrt::Microsoft::ReactNative::JSValueObject GetExifObject(
      winrt::Windows::Graphics::Imaging::BitmapPropertySet const &properties) noexcept;

  bool TryGetValueAsInt(
      winrt::Microsoft::ReactNative::JSValueObject const &options,
      const std::string key,
      int &value,
      const int defaultValue) noexcept;

  bool TryGetValueAsBool(
      winrt::Microsoft::ReactNative::JSValueObject const &options,
      const std::string key,
      bool &value,
      const bool defaultValue) noexcept;

  bool TryGetValueAsFloat(
      winrt::Microsoft::ReactNative::JSValueObject const &options,
      const std::string key,
      float &value,
      const float defaultValue) noexcept;

  winrt::Microsoft::ReactNative::IReactContext m_reactContext{nullptr};
  winrt::Windows::UI::Xaml::Controls::CaptureElement m_childElement;

  handle m_signal{CreateEvent(nullptr, true, false, nullptr)};
  winrt::Windows::Media::Capture::LowLagMediaRecording m_mediaRecording{nullptr};
  winrt::ReactNativeCameraCPP::CameraRotationHelper m_rotationHelper{nullptr};
  winrt::Windows::System::Display::DisplayRequest m_displayRequest{nullptr};

  winrt::Windows::System::Threading::ThreadPoolTimer m_recordTimer{nullptr};
  winrt::Windows::System::Threading::ThreadPoolTimer m_barcodeScanTimer{nullptr};
  winrt::Windows::System::Threading::ThreadPoolTimer m_faceDetectTimer{nullptr};
  winrt::Windows::System::Threading::ThreadPoolTimer m_objectDetectTimer{nullptr};

  winrt::event_token m_rotationEventToken{};
  winrt::Windows::UI::Xaml::Application::Suspending_revoker m_applicationSuspendingEventToken;
  winrt::Windows::UI::Xaml::Application::Resuming_revoker m_applicationResumingEventToken;
  winrt::Windows::UI::Xaml::FrameworkElement::Unloaded_revoker m_unloadedEventToken;

  // References a FaceDetector instance.
  winrt::Windows::Media::FaceAnalysis::FaceDetector m_faceDetector{nullptr};

  winrt::Microsoft::AI::Skills::Vision::ObjectDetector::ObjectDetectorDescriptor m_skillDescriptor{nullptr};
  winrt::Microsoft::AI::Skills::Vision::ObjectDetector::ObjectDetectorSkill m_skill{nullptr};
  winrt::Microsoft::AI::Skills::Vision::ObjectDetector::ObjectDetectorBinding m_binding{nullptr};

  bool m_isInitialized{false};
  bool m_keepAwake{false};
  bool m_isRecording{false};
  int m_flashMode{ReactCameraConstants::CameraFlashModeOff};
  int m_whiteBalance{ReactCameraConstants::CameraWhiteBalanceAuto};
  int m_focusMode{ReactCameraConstants::CameraAutoFocusOn};
  bool m_isPreview{false};
  bool m_mirrorVideo{false};
  bool m_barcodeScannerEnabled{false};
  bool m_faceDetectEnabled{false};
  bool m_objectDetectEnabled{false};

  int m_barcodeReadIntervalMS{ReactCameraConstants::BarcodeReadIntervalMS};
  int m_detectReadIntervalMS{ReactCameraConstants::DetectReadIntervalMS};

  int m_videoWidht{0};
  int m_videoHeight{0};
  
  std::string m_cameraId;

  winrt::Windows::Devices::Enumeration::Panel m_panelType{winrt::Windows::Devices::Enumeration::Panel::Unknown};

  winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Media::MediaProperties::IMediaEncodingProperties>
      m_availableVideoEncodingProperties;

  std::vector<winrt::ZXing::BarcodeType> m_barcodeTypes;

  int m_defaultVideoQuality{ReactCameraConstants::CameraVideoQualityAuto};

};
} // namespace winrt::ReactNativeCameraCPP