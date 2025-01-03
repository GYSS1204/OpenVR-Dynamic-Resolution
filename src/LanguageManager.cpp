#include "LanguageManager.h"

LanguageManager::LanguageManager() {
    currentLanguage = ENGLISH;
    initializeTranslations();
}

void LanguageManager::initializeTranslations() {
    // Define translations for different keys
    translations = {
        {"app_title", {
            {ENGLISH, "OVR Dynamic Resolution"},
            {SIMPLIFIED_CHINESE, "OVR 动态分辨率"},
            {JAPANESE, "OVR ダイナミック解像度"}
        }},
        {"hmd_refresh_rate", {
            {ENGLISH, "HMD refresh rate: {} hz ({:.2f} ms)"},
            {SIMPLIFIED_CHINESE, "HMD刷新率：{} 赫兹 ({:.2f} 毫秒)"},
            {JAPANESE, "HMD リフレッシュレート：{} Hz ({:.2f} ミリ秒)"}
        }},
        {"target_fps", {
            {ENGLISH, "Target FPS: {} fps ({:.2f} ms)"},
            {SIMPLIFIED_CHINESE, "目标帧率：{} 帧/秒 ({:.2f} 毫秒)"},
            {JAPANESE, "目標FPS：{} fps ({:.2f} ミリ秒)"}
        }},
        {"target_fps_disabled", {
            {ENGLISH, "Target FPS: Disabled"},
            {SIMPLIFIED_CHINESE, "目标帧率：已禁用"},
            {JAPANESE, "目標FPS：無効"}
        }},
        {"target_VRAM", {
            {ENGLISH, "Target VRAM: {:.2f} GB"},
            {SIMPLIFIED_CHINESE, "目标显存：{:.2f} GB"},
            {JAPANESE, "ターゲットVRAM：{:.2f} GB"}
        }},
        {"target_VRAM_disabled", {
            {ENGLISH, "Target VRAM: Disabled"},
            {SIMPLIFIED_CHINESE, "目标显存：已禁用"},
            {JAPANESE, "ターゲットVRAM：無効"}
        }},
        {"limit_VRAM", {
            {ENGLISH, "Limit VRAM: {:.2f} GB"},
            {SIMPLIFIED_CHINESE, "限制显存：{:.2f} GB"},
            {JAPANESE, "VRAM制限：{:.2f} GB"}
        }},
        {"limit_VRAM_disabled", {
            {ENGLISH, "Limit VRAM: Disabled"},
            {SIMPLIFIED_CHINESE, "限制显存：已禁用"},
            {JAPANESE, "VRAM制限：無効"}
        }},
        {"GPU_frametime", {
            {ENGLISH, "GPU frametime: {:.2f} ms"},
            {SIMPLIFIED_CHINESE, "GPU帧时间：{:.2f} 毫秒"},
            {JAPANESE, "GPU フレームタイム：{:.2f} ミリ秒"}
        }},
        {"CPU_frametime", {
            {ENGLISH, "CPU frametime: {:.2f} ms"},
            {SIMPLIFIED_CHINESE, "CPU帧时间：{:.2f} 毫秒"},
            {JAPANESE, "CPU フレームタイム：{:.2f} ミリ秒"}
        }},
        {"VRAM_usage", {
            {ENGLISH, "VRAM usage: {:.2f}/{:.2f} GB ({}%)"},
            {SIMPLIFIED_CHINESE, "GPU显存使用量：{:.2f}/{:.2f} GB ({}%)"},
            {JAPANESE, "VRAM使用量：{:.2f}/{:.2f} GB ({}%)"}
        }},
        {"VRAM_usage_disabled", {
            {ENGLISH, "VRAM usage: Disabled"},
            {SIMPLIFIED_CHINESE, "显存使用：已关闭"},
            {JAPANESE, "VRAM使用：無効"}
        }},
        {"GPU_usage", {
            {ENGLISH, "GPU usage: {} %"},
            {SIMPLIFIED_CHINESE, "GPU使用率：{} %"},
            {JAPANESE, "GPU使用率：{} %"}
        }},
        {"Reprojection_ratio", {
            {ENGLISH, "Reprojection ratio: {:.2f}"},
            {SIMPLIFIED_CHINESE, "重新渲染比率：{:.2f}"},
            {JAPANESE, "再投影比率：{:.2f}"}
        }},
        {"Resolution_info", {
            {ENGLISH, "Resolution = {}"},
            {SIMPLIFIED_CHINESE, "分辨率 = {}"},
            {JAPANESE, "解像度 = {}"}
        }},
        {"Resolution_info_2", {
            {ENGLISH, "Resolution ="},
            {SIMPLIFIED_CHINESE, "分辨率 ="},
            {JAPANESE, "解像度 ="}
        }},
        {"Resolution", {
            {ENGLISH, "Resolution"},
            {SIMPLIFIED_CHINESE, "分辨率"},
            {JAPANESE, "解像度"}
        }},
        {"Adjustment_paused", {
            {ENGLISH, "(adjustment paused)"},
            {SIMPLIFIED_CHINESE, "（调整已暂停）"},
            {JAPANESE, "(調整が一時停止中)"}
        }},
        {"Settings", {
            {ENGLISH, "Settings"},
            {SIMPLIFIED_CHINESE, "设置"},
            {JAPANESE, "設定"}
        }},
        {"Manual_resolution", {
            {ENGLISH, "Manual resolution"},
            {SIMPLIFIED_CHINESE, "手动分辨率"},
            {JAPANESE, "手動解像度"}
        }},
        {"Dynamic_resolution", {
            {ENGLISH, "Dynamic resolution"},
            {SIMPLIFIED_CHINESE, "动态分辨率"},
            {JAPANESE, "ダイナミック解像度"}
        }},
        {"Startup", {
            {ENGLISH, "Startup"},
            {SIMPLIFIED_CHINESE, "启动"},
            {JAPANESE, "起動"}
        }},
        {"FPS", {
            {ENGLISH, "FPS: {} fps"},
            {SIMPLIFIED_CHINESE, "帧率: {} fps"},
            {JAPANESE, "FPS：{} fps"}
        }},
        {"Startup_behavior", {
            {ENGLISH, "Startup behaviour:"},
            {SIMPLIFIED_CHINESE, "启动行为："},
            {JAPANESE, "起動時の動作："}
        }},
        {"General", {
            {ENGLISH, "General"},
            {SIMPLIFIED_CHINESE, "通用"},
            {JAPANESE, "一般"}
        }},
        {"Select_language", {
            {ENGLISH, "Language"},
            {SIMPLIFIED_CHINESE, "语言"},
            {JAPANESE, "言語"}
        }},
        {"Tooltip_select_language", {
            {ENGLISH, "Select to switch languages"},
            {SIMPLIFIED_CHINESE, "选择以切换语言"},
            {JAPANESE, "言語を切り替えるには選択してください"}
        }},
        {"Visible", {
            {ENGLISH, "Visible"},
            {SIMPLIFIED_CHINESE, "可见"},
            {JAPANESE, "表示"}
        }},
        {"Minimized_taskbar", {
            {ENGLISH, "Minimized (taskbar)"},
            {SIMPLIFIED_CHINESE, "最小化（任务栏）"},
            {JAPANESE, "最小化（タスクバー）"}
        }},
        {"Hidden_tray", {
            {ENGLISH, "Hidden (tray)"},
            {SIMPLIFIED_CHINESE, "隐藏（托盘）"},
            {JAPANESE, "非表示（トレイ）"}
        }},
        {"Start_with_SteamVR", {
            {ENGLISH, "Start with SteamVR"},
            {SIMPLIFIED_CHINESE, "随SteamVR启动"},
            {JAPANESE, "SteamVRと一緒に起動"}
        }},
        {"Tooltip_start_with_SteamVR", {
            {ENGLISH, "Automatically launch OVRDR alongside SteamVR."},
            {SIMPLIFIED_CHINESE, "自动与SteamVR一起启动OVRDR。"},
            {JAPANESE, "SteamVRと一緒にOVRDRを自動的に起動します。"}
        }},
        {"Tooltip_visible", {
            {ENGLISH, "Keep the OVRDR window visible on startup."},
            {SIMPLIFIED_CHINESE, "启动时保持OVRDR窗口可见。"},
            {JAPANESE, "起動時にOVRDRウィンドウを表示したままにします。"}
        }},
        {"Tooltip_minimized_taskbar", {
            {ENGLISH, "Minimize the OVRDR window to the taskbar on startup."},
            {SIMPLIFIED_CHINESE, "启动时将OVRDR窗口最小化到任务栏。"},
            {JAPANESE, "起動時にOVRDRウィンドウをタスクバーに最小化します。"}
        }},
        {"Tooltip_hidden_tray", {
            {ENGLISH, "Hide the OVRDR window completely on startup. You can still show the window by clicking \"Show\" in the tray icon's context menu."},
            {SIMPLIFIED_CHINESE, "启动时完全隐藏OVRDR窗口。您仍然可以通过单击托盘图标的\"显示\"选项来显示窗口。"},
            {JAPANESE, "起動時にOVRDRウィンドウを完全に非表示にします。トレイアイコンのコンテキストメニューで「表示」をクリックすることでウィンドウを表示できます。"}
        }},
        {"Resolution_change_delay_ms", {
            {ENGLISH, "Resolution change delay ms"},
            {SIMPLIFIED_CHINESE, "分辨率变化延迟(毫秒)"},
            {JAPANESE, "解像度変更遅延（ミリ秒）"}
        }},
        {"Tooltip_resolution_change_delay_ms", {
            {ENGLISH, "Delay in milliseconds between resolution changes."},
            {SIMPLIFIED_CHINESE, "分辨率变化之间的延迟（以毫秒为单位）。"},
            {JAPANESE, "解像度変更間の遅延時間（ミリ秒単位）。"}
        }},
        {"Data_average_samples", {
            {ENGLISH, "Data average samples."},
            {SIMPLIFIED_CHINESE, "数据平均样本。"},
            {JAPANESE, "データ平均サンプル数。"}
        }},
        {"Tooltip_data_average_samples", {
            {ENGLISH, "Number of frames' frametimes to average out."},
            {SIMPLIFIED_CHINESE, "要平均的帧数。"},
            {JAPANESE, "平均するフレームのフレームタイム数。"}
        }},
        {"Disable_current_application", {
            {ENGLISH, "Disable current application"},
            {SIMPLIFIED_CHINESE, "禁用当前应用程序"},
            {JAPANESE, "現在のアプリケーションを無効にする"}
        }},
        {"External_res_change_compatibility", {
            {ENGLISH, "External res change compatibility"},
            {SIMPLIFIED_CHINESE, "外部资源变更兼容性"},
            {JAPANESE, "外部解像度変更の互換性"}
        }},
        {"Tooltip_external_res_change_compatibility", {
            {ENGLISH, "Automatically switch to manual resolution adjustment within the app when VR resolution is changed from an external source (SteamVR setting, Oyasumi, etc.) as to let the external source control the resolution. Does not automatically switch back to dynamic resolution adjustment."},
            {SIMPLIFIED_CHINESE, "当 VR 分辨率从外部源（SteamVR 设置、Oyasumi 等）更改时，自动在应用程序内切换到手动分辨率调整，以便让外部源控制分辨率。不会自动切换回动态分辨率调整。"},
            {JAPANESE, "外部ソース（SteamVR設定、おやすみなど）からVR解像度が変更された場合、外部ソースが解像度を制御できるように、アプリ内で手動解像度調整に自動的に切り替えます。動的解像度調整に自動的に切り替わりません。"}
        }},
        {"Blacklist", {
            {ENGLISH, "Blacklist"},
            {SIMPLIFIED_CHINESE, "黑名单"},
            {JAPANESE, "ブラックリスト"}
        }},
        {"Tooltip_blacklist", {
            {ENGLISH, "Don't allow resolution changes in blacklisted applications."},
            {SIMPLIFIED_CHINESE, "不允许在黑名单应用程序中更改分辨率。"},
            {JAPANESE, "ブラックリストに登録されたアプリケーションでは解像度の変更を許可しません。"}
        }},
        {"Blacklisted_apps", {
            {ENGLISH, "Blacklisted apps"},
            {SIMPLIFIED_CHINESE, "列入黑名单的应用程序"},
            {JAPANESE, "ブラックリストに登録されたアプリ"}
        }},
        {"Tooltip_blacklisted_apps", {
            {ENGLISH, "List of OpenVR application keys that should be blacklisted for resolution adjustment in the format \'steam.app.APPID\' (e.g. \'steam.app.620980\' for Beat Saber). One per line."},
            {SIMPLIFIED_CHINESE, "应列入黑名单以进行分辨率调整的 OpenVR 应用程序密钥列表，格式为 \'steam.app.APPID\'（例如 Beat Saber 为 \'steam.app.620980\'）。每行一个。"},
            {JAPANESE, "解像度調整のためにブラックリストに登録する必要がある OpenVR アプリケーション キーのリスト。形式は \'steam.app.APPID\' (例: Beat Saber の場合は \'steam.app.620980\') です。1 行に 1 つずつ。"}
        }},
        {"Blacklist_current_app", {
            {ENGLISH, "Blacklist current app"},
            {SIMPLIFIED_CHINESE, "将当前应用加入黑名单"},
            {JAPANESE, "現在のアプリをブラックリストに追加する"}
        }},
        {"Tooltip_blacklist_current_app", {
            {ENGLISH, "Adds the current application to the blacklist."},
            {SIMPLIFIED_CHINESE, "将当前应用程序添加到黑名单。"},
            {JAPANESE, "現在のアプリケーションをブラックリストに追加します。"}
        }},
        {"Enable_whitelist", {
            {ENGLISH, "Enable whitelist"},
            {SIMPLIFIED_CHINESE, "启用白名单"},
            {JAPANESE, "ホワイトリストを有効にする"}
        }},
        {"Tooltip_enable_whitelist", {
            {ENGLISH, "Only allow resolution changes in whitelisted applications."},
            {SIMPLIFIED_CHINESE, "仅允许在白名单应用程序中更改分辨率。"},
            {JAPANESE, "ホワイトリストに登録されたアプリケーションでのみ解像度の変更を許可します。"}
        }},
        {"Whitelisted_apps", {
            {ENGLISH, "Whitelisted apps"},
            {SIMPLIFIED_CHINESE, "列入白名单的应用程序"},
            {JAPANESE, "ホワイトリストに登録されたアプリ"}
        }},
        {"Tooltip_whitelisted_apps", {
            {ENGLISH, "List of OpenVR application keys that should be whitelisted for resolution adjustment in the format \'steam.app.APPID\' (e.g. \'steam.app.620980\' for Beat Saber). One per line."},
            {SIMPLIFIED_CHINESE, "应列入白名单以进行分辨率调整的 OpenVR 应用程序密钥列表，格式为 \'steam.app.APPID\'（例如 Beat Saber 为 \'steam.app.620980\'）。每行一个。"},
            {JAPANESE, "解像度調整のためにホワイトリストに登録する必要がある OpenVR アプリケーション キーのリスト。形式は \'steam.app.APPID\' (例: Beat Saber の場合は \'steam.app.620980\')。1 行に 1 つずつ。"}
        }},
        {"Whitelist_current_app", {
            {ENGLISH, "Whitelist current app"},
            {SIMPLIFIED_CHINESE, "将当前应用加入白名单"},
            {JAPANESE, "現在のアプリをホワイトリストに追加する"}
        }},
        {"Tooltip_whitelisted_current_app", {
            {ENGLISH, "Adds the current application to the blacklist."},
            {SIMPLIFIED_CHINESE, "将当前应用程序添加到白名单。"},
            {JAPANESE, "現在のアプリケーションをホワイトリストに追加します。"}
        }},
        {"Initial_resolution", {
            {ENGLISH, "Initial resolution"},
            {SIMPLIFIED_CHINESE, "初始分辨率"},
            {JAPANESE, "初期解像度"}
        }},
        {"Tooltip_initial_resolution", {
            {ENGLISH, "The resolution set at startup. Also used when resetting resolution."},
            {SIMPLIFIED_CHINESE, "启动时设置的分辨率。也用于重置分辨率时。"},
            {JAPANESE, "起動時に設定される解像度。解像度をリセットする際にも使用されます。"}
        }},
        {"Minimum_resolution", {
            {ENGLISH, "Minimum resolution"},
            {SIMPLIFIED_CHINESE, "最小分辨率"},
            {JAPANESE, "最小解像度"}
        }},
        {"Tooltip_minimum_resolution", {
            {ENGLISH, "The minimum resolution OVRDR will set."},
            {SIMPLIFIED_CHINESE, "OVRDR将设置的最小分辨率。"},
            {JAPANESE, "OVRDRが設定する最小解像度。"}
        }},
        {"Maximum_resolution", {
            {ENGLISH, "Maximum resolution"},
            {SIMPLIFIED_CHINESE, "最大分辨率"},
            {JAPANESE, "最大解像度"}
        }},
        {"Tooltip_maximum_resolution", {
            {ENGLISH, "The maximum resolution OVRDR will set."},
            {SIMPLIFIED_CHINESE, "OVRDR将设置的最大分辨率。"},
            {JAPANESE, "OVRDRが設定する最大解像度。"}
        }},
        {"Advanced", {
            {ENGLISH, "Advanced"},
            {SIMPLIFIED_CHINESE, "高级"},
            {JAPANESE, "詳細設定"}
        }},
        {"Increase_threshold", {
            {ENGLISH, "Increase threshold(fps)"},
            {SIMPLIFIED_CHINESE, "增加阈值(fps)"},
            {JAPANESE, "増加閾値(fps)"}
        }},
        {"Tooltip_increase_threshold", {
            {ENGLISH, "When the frame rate is greater than or equal to this value, the resolution starts to increase."},
            {SIMPLIFIED_CHINESE, "当帧率大于等于该值时开始增加分辨率。"},
            {JAPANESE, "フレーム レートがこの値以上になったら、解像度の増加を開始します。"}
        }},
        {"Decrease_threshold", {
            {ENGLISH, "Decrease threshold(fps)"},
            {SIMPLIFIED_CHINESE, "减少阈值(fps)"},
            {JAPANESE, "減少閾値(fps)"}
        }},
        {"Tooltip_decrease_threshold", {
            {ENGLISH, "When the frame rate drops below this value, the resolution starts to decrease."},
            {SIMPLIFIED_CHINESE, "当帧率小于该值时开始降低分辨率。"},
            {JAPANESE, "フレーム レートがこの値より低い場合は、解像度の低下を開始します。"}
        }},
        {"Increase_minimum", {
            {ENGLISH, "Increase minimum"},
            {SIMPLIFIED_CHINESE, "最小增加量"},
            {JAPANESE, "最小増加量"}
        }},
        {"Tooltip_increase_minimum", {
            {ENGLISH, "Percentages to increase resolution when available."},
            {SIMPLIFIED_CHINESE, "每次增加分辨率的百分比。"},
            {JAPANESE, "利用可能なときに解像度を増加させる割合。"}
        }},
        {"Decrease_minimum", {
            {ENGLISH, "Decrease minimum"},
            {SIMPLIFIED_CHINESE, "最小减少量"},
            {JAPANESE, "最小減少量"}
        }},
        {"Tooltip_decrease_minimum", {
            {ENGLISH, "Percentages to decrease resolution when needed."},
            {SIMPLIFIED_CHINESE, "每次减少分辨率的百分比。"},
            {JAPANESE, "必要に応じて解像度を減少させる割合。"}
        }},
        {"Increase_scale", {
            {ENGLISH, "Increase scale"},
            {SIMPLIFIED_CHINESE, "增加比例"},
            {JAPANESE, "増加スケール"}
        }},
        {"Tooltip_increase_scale", {
            {ENGLISH, "The more frametime headroom and the higher this value is, the more resolution will increase each time."},
            {SIMPLIFIED_CHINESE, "帧时间余量越多，此值越高，每次分辨率的增加越多。"},
            {JAPANESE, "フレームタイムの余裕が多く、この値が高いほど、解像度が増加する度合いが大きくなります。"}
        }},
        {"Decrease_scale", {
            {ENGLISH, "Decrease scale"},
            {SIMPLIFIED_CHINESE, "减少比例"},
            {JAPANESE, "減少スケール"}
        }},
        {"Tooltip_decrease_scale", {
            {ENGLISH, "The more frametime excess and the higher this value is, the more resolution will decrease each time."},
            {SIMPLIFIED_CHINESE, "帧时间超出越多，此值越高，每次分辨率的减少越多。"},
            {JAPANESE, "フレームタイムの超過が多く、この値が高いほど、解像度が減少する度合いが大きくなります。"}
        }},
        {"Minimum_CPU_time_threshold", {
            {ENGLISH, "Minimum CPU time threshold"},
            {SIMPLIFIED_CHINESE, "最小CPU时间阈值"},
            {JAPANESE, "最小CPU時間閾値"}
        }},
        {"Tooltip_minimum_CPU_time_threshold", {
            {ENGLISH, "Don't increase resolution if the CPU frametime is below this value (useful to prevent resolution increases during loading screens)."},
            {SIMPLIFIED_CHINESE, "如果CPU帧时间低于此值，则不增加分辨率（可防止加载屏幕期间增加分辨率）。"},
            {JAPANESE, "CPUフレームタイムがこの値未満の場合、解像度を増加させません（ロード画面での解像度の増加を防ぐのに役立ちます）。"}
        }},
        {"Reset_on_CPU_time_threshold", {
            {ENGLISH, "Reset on CPU time threshold"},
            {SIMPLIFIED_CHINESE, "CPU时间阈值触发重置"},
            {JAPANESE, "CPU時間閾値でリセット"}
        }},
        {"Tooltip_reset_on_CPU_time_threshold", {
            {ENGLISH, "Reset the resolution to the initial resolution whenever the \"Minimum CPU time threshold\" is met."},
            {SIMPLIFIED_CHINESE, "每当达到\"最小CPU时间阈值\"时，将分辨率重置为初始值。"},
            {JAPANESE, "\"最小CPU時間閾値\"が満たされるたびに、解像度を初期解像度にリセットします。"}
        }},
        {"header_reprojection", {
            {ENGLISH, "Reprojection"},
            {SIMPLIFIED_CHINESE, "重新采样"},
            {JAPANESE, "再投影"}
        }},
        {"Always_reproject", {
            {ENGLISH, "Always reproject"},
            {SIMPLIFIED_CHINESE, "始终重新采样"},
            {JAPANESE, "常に再投影"}
        }},
        {"Tooltip_always_reproject", {
            {ENGLISH, "Always double the target frametime."},
            {SIMPLIFIED_CHINESE, "始终将目标帧时间加倍。"},
            {JAPANESE, "常にターゲットフレームタイムを2倍にします。"}
        }},
        {"Prefer_reprojection", {
            {ENGLISH, "Prefer reprojection"},
            {SIMPLIFIED_CHINESE, "优先重新采样"},
            {JAPANESE, "再投影を優先"}
        }},
        {"Tooltip_prefer_reprojection", {
            {ENGLISH, "If enabled, double the target frametime as soon as the CPU frametime is over the initial target frametime. Else, only double the target frametime if the CPU frametime is over double the initial target frametime."},
            {SIMPLIFIED_CHINESE, "启用时，只要CPU帧时间超过初始目标帧时间，就加倍目标帧时间。否则，只有当CPU帧时间超过目标帧时间的两倍时才加倍。"},
            {JAPANESE, "有効にすると、CPUフレームタイムが初期ターゲットフレームタイムを超えた時点でターゲットフレームタイムを2倍にします。それ以外の場合は、CPUフレームタイムが初期ターゲットフレームタイムの2倍を超えた場合のみターゲットフレームタイムを2倍にします。"}
        }},
        {"Ignore_CPU_time", {
            {ENGLISH, "Ignore CPU time"},
            {SIMPLIFIED_CHINESE, "忽略CPU时间"},
            {JAPANESE, "CPU時間を無視する"}
        }},
        {"Tooltip_ignore_CPU_time", {
            {ENGLISH, "Never change the target frametime depending on the CPU frametime (stops both behaviours described in \"Prefer reprojection\" tooltip)."},
            {SIMPLIFIED_CHINESE, "不要根据CPU帧时间更改目标帧时间（停止\"优先重投影\"工具提示中描述的两种行为）。"},
            {JAPANESE, "CPUのフレームタイムに依存してターゲットフレームタイムを変更しない（「再投影を優先」ツールチップで説明されている2つの動作を停止します）。"}
        }},
        {"Save", {
            {ENGLISH, "Save"},
            {SIMPLIFIED_CHINESE, "保存"},
            {JAPANESE, "保存"}
        }},
        {"Revert", {
            {ENGLISH, "Revert"},
            {SIMPLIFIED_CHINESE, "还原"},
            {JAPANESE, "元に戻す"}
        }},
        {"Close", {
            {ENGLISH, "Close"},
            {SIMPLIFIED_CHINESE, "关闭"},
            {JAPANESE, "閉じる"}
        }},
        {"Tooltip_save", {
            {ENGLISH, "Save the current settings."},
            {SIMPLIFIED_CHINESE, "保存当前设置。"},
            {JAPANESE, "現在の設定を保存する。"}
        }},
        {"Tooltip_revert", {
            {ENGLISH, "Revert to the previous settings."},
            {SIMPLIFIED_CHINESE, "还原到之前的设置。"},
            {JAPANESE, "前の設定に戻す。"}
        }},
        {"Tooltip_close", {
            {ENGLISH, "Close the settings menu."},
            {SIMPLIFIED_CHINESE, "关闭设置菜单。"},
            {JAPANESE, "設定メニューを閉じる。"}
        }},
        {"VRAM", {
            {ENGLISH, "VRAM"},
            {SIMPLIFIED_CHINESE, "显存"},
            {JAPANESE, "VRAM"}
        }},
        {"VRAM-only_mode", {
            {ENGLISH, "VRAM-only mode"},
            {SIMPLIFIED_CHINESE, "仅使用VRAM模式"},
            {JAPANESE, "VRAM専用モード"}
        }},
        {"Tooltip_VRAM-only_mode", {
            {ENGLISH, "Always stay at the initial resolution or lower based off available VRAM alone (ignoring frametimes)."},
            {SIMPLIFIED_CHINESE, "始终保持基于可用VRAM的初始分辨率或更低的分辨率（忽略帧时间）。"},
            {JAPANESE, "利用可能なVRAMのみを基に、初期解像度またはそれ以下に常に維持する（フレームタイムは無視）。"}
        }},
        {"VRAM_target", {
            {ENGLISH, "VRAM target"},
            {SIMPLIFIED_CHINESE, "目标显存大小"},
            {JAPANESE, "VRAMターゲット"}
        }},
        {"VRAM_limit", {
            {ENGLISH, "VRAM limit"},
            {SIMPLIFIED_CHINESE, "限制显存大小"},
            {JAPANESE, "VRAM制限"}
        }},
        {"VRAM_monitor_enabled", {
            {ENGLISH, "VRAM monitor enabled"},
            {SIMPLIFIED_CHINESE, "启用显存监控"},
            {JAPANESE, "VRAMモニターが有効"}
        }},
        {"Tooltip_vram_monitor", {
            {ENGLISH, "Enable VRAM specific features. If disabled, it is assumed that free VRAM is always available."},
            {SIMPLIFIED_CHINESE, "启用显存相关功能。如果禁用，假定显存总是可用。"},
            {JAPANESE, "VRAMに特化した機能を有効にします。無効にすると、空きVRAMが常に利用可能であると見なされます。"}
        }},
        {"Tooltip_vram_limit", {
            {ENGLISH, "Resolution starts decreasing once VRAM usage exceeds this percentage."},
            {SIMPLIFIED_CHINESE, "当显存使用率超过此百分比时，分辨率开始下降。"},
            {JAPANESE, "VRAMの使用率がこのパーセンテージを超えると、解像度が低下し始めます。"}
        }},
        {"Tooltip_vram_target", {
            {ENGLISH, "Resolution stops increasing once VRAM usage exceeds this percentage."},
            {SIMPLIFIED_CHINESE, "当显存使用率超过此百分比时，分辨率停止增加。"},
            {JAPANESE, "VRAMの使用率がこのパーセンテージを超えると、解像度の増加が停止します。"}
        }},

        {"GPU_usage_b", {
            {ENGLISH, "GPU usage"},
            {SIMPLIFIED_CHINESE, "GPU使用率"},
            {JAPANESE, "GPUの使用率"}
        }},
        {"GPU_usage_target", {
            {ENGLISH, "GPU usage target"},
            {SIMPLIFIED_CHINESE, "目标GPU使用率"},
            {JAPANESE, "ターゲットの GPU 使用率"}
        }},
        {"GPU_usage_limit", {
            {ENGLISH, "GPU usage limit"},
            {SIMPLIFIED_CHINESE, "限制GPU使用率"},
            {JAPANESE, "GPUの使用量を制限する"}
        }},
        {"GPU_usage_enabled", {
            {ENGLISH, "GPU usage enabled"},
            {SIMPLIFIED_CHINESE, "启用GPU使用率"},
            {JAPANESE, "GPUの使用を有効にする"}
        }},
        {"Tooltip_GPU_usage_enabled", {
            {ENGLISH, "Enable GPU usage related features."},
            {SIMPLIFIED_CHINESE, "启用GPU使用率相关功能"},
            {JAPANESE, "GPU 使用率関連の機能を有効にする"}
        }},
        {"Tooltip_GPU_usage_limit", {
            {ENGLISH, "When GPU usage exceeds this percentage, the resolution stops increasing."},
            {SIMPLIFIED_CHINESE, "当GPU使用率超过此百分比时，分辨率停止增加"},
            {JAPANESE, "GPU 使用率がこのパーセンテージを超えると、解像度の増加が停止します"}
        }},
        {"Tooltip_GPU_usage_target", {
            {ENGLISH, "When GPU usage falls below this percentage, the resolution stops decreasing."},
            {SIMPLIFIED_CHINESE, "当GPU使用率低于此百分比时，分辨率停止下降"},
            {JAPANESE, "GPU 使用率がこのパーセンテージを下回ると、解像度の低下が停止します"}
        }},
        {"RAM", {
            {ENGLISH, "RAM"},
            {SIMPLIFIED_CHINESE, "内存"},
            {JAPANESE, "RAM"}
        }},
        {"RAM_monitor_enabled", {
            {ENGLISH, "RAM monitor enabled"},
            {SIMPLIFIED_CHINESE, "启用内存监控"},
            {JAPANESE, "メモリ監視を有効にする"}
        }},
        {"Tooltip_ram_monitor", {
            {ENGLISH, "Enables memory related features. If disabled, memory is assumed to be always available."},
            {SIMPLIFIED_CHINESE, "启用内存相关功能。如果禁用，假定内存总是可用。"},
            {JAPANESE, "メモリ関連の機能を有効にします。無効にすると、メモリは常に使用可能であるとみなされます。"}
        }},
        {"RAM_limit", {
            {ENGLISH, "RAM limit"},
            {SIMPLIFIED_CHINESE, "限制内存大小"},
            {JAPANESE, "メモリサイズを制限する"}
        }},
        {"Tooltip_ram_limit", {
            {ENGLISH, "When memory usage exceeds this value, the resolution stops changing."},
            {SIMPLIFIED_CHINESE, "内存使用率超过此值时，分辨率停止改变。"},
            {JAPANESE, "メモリ使用量がこの値を超えると、解像度の変更が停止します。"}
        }},
        {"RAM_usage", {
            {ENGLISH, "RAM usage: {:.2f}/{:.2f} GB ({}%)"},
            {SIMPLIFIED_CHINESE, "内存使用量：{:.2f}/{:.2f} GB ({}%)"},
            {JAPANESE, "RAM使用量：{:.2f}/{:.2f} GB ({}%)"}
        }},


        
        // Add more translations here
    };
}

void LanguageManager::setLanguage(Language lang) {
    currentLanguage = lang;
}

std::string LanguageManager::translate(const std::string& key) {
    auto it = translations.find(key);
    if (it != translations.end()) {
        return it->second[currentLanguage];
    }
    return key; // Fallback to original key
}

// Usage in main code
// Replace ImGui text rendering:
// ImGui::Text("%s", fmt::format(LanguageManager::getInstance().translate("hmd_refresh_rate"), hmdHz, hmdFrametime).c_str());



#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
//#include <ImGuiFileDialog.h>

// 使用 stb_truetype.h 嵌入字体
#define STB_TRUETYPE_IMPLEMENTATION
//#include "stb_truetype.h"

// 中文字体加载函数
bool LoadChineseFonts(ImGuiIO& io) {
    // 方法1：使用系统预装的中文字体
    const char* fontPaths[] = {
        // Windows 常见中文字体路径
        // "C:\\Windows\\Fonts\\simhei.ttf",    // 黑体
        // "C:\\Windows\\Fonts\\simsun.ttc",    // 宋体
        "C:\\Windows\\Fonts\\msyh.ttc",      // 微软雅黑
    };

    for (const char* fontPath : fontPaths) {
        FILE* fontFile = fopen(fontPath, "rb");
        if (fontFile) {
            fseek(fontFile, 0, SEEK_END);
            long fileSize = ftell(fontFile);
            fseek(fontFile, 0, SEEK_SET);

            unsigned char* fontBuffer = new unsigned char[fileSize];
            fread(fontBuffer, 1, fileSize, fontFile);
            fclose(fontFile);

            ImFontConfig config;
            config.MergeMode = true; // 合并到默认字体
            config.GlyphMinAdvanceX = 13.0f; // 中文字符宽度

            // 加载字体并支持中文 Unicode 范围
            io.Fonts->AddFontFromFileTTF(
                fontPath, 
                14.0f,  // 字体大小
                &config, 
                io.Fonts->GetGlyphRangesChineseSimplifiedCommon()
            );
            // 支持更多 Unicode 范围，确保特殊符号正常显示
            static const ImWchar ranges[] = {
                0x0020, 0x00FF, // 基本拉丁文
                0x2000, 0x206F, // 常用标点
                0x3000, 0x30FF, // 日文假名和符号
                0x4E00, 0x9FAF, // CJK 统一汉字
                0xFF00, 0xFFEF, // 全角字符
                0,
            };
            io.Fonts->AddFontFromFileTTF(
                fontPath, 
                16.0f, 
                &config, 
                ranges
            );

             // 重建字体纹理
            io.Fonts->Build();

            return true; // 找到并加载成功
        }
    }

    // 方法2：如果系统字体都不可用，可以考虑内嵌字体资源
    // 这里需要将字体文件编译到程序中
    return false;
}

// 在 ImGui 初始化时调用
void SetupImGuiFont() {
    ImGuiIO& io = ImGui::GetIO();
    
    // 首先加载默认英文字体
    io.Fonts->AddFontDefault();

    // 尝试加载中文字体
    if (!LoadChineseFonts(io)) {
        // 如果加载失败，可以提供备选方案
        printf("Warning: No Chinese font found. Falling back to default font.\n");
    }

    // 重新构建字体纹理
    io.Fonts->Build();
}

// 在 main 函数或初始化阶段调用
void InitImGuiWithChineseFonts() {
    // ImGui 初始化
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    // 设置中文字体
    SetupImGuiFont();

    // 其他 ImGui 初始化代码...
}

void setLanguage(int language) {
    switch (language)
    {
    case LanguageManager::ENGLISH:
        LanguageManager::getInstance().setLanguage(LanguageManager::ENGLISH);
        break;
    case LanguageManager::SIMPLIFIED_CHINESE:
        LanguageManager::getInstance().setLanguage(LanguageManager::SIMPLIFIED_CHINESE);
        break;
    case LanguageManager::JAPANESE:
        LanguageManager::getInstance().setLanguage(LanguageManager::JAPANESE);
        break;
    default:
    LanguageManager::getInstance().setLanguage(LanguageManager::ENGLISH);
        break;
    }
}