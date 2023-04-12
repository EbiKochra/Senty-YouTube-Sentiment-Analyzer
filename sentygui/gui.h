#ifndef GUI_H
#define GUI_H

#include "iView.h"
#include "appModel.h"
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

// static void glfw_error_callback(int error, const char* description) {
//     fprintf(stderr, "GLFW Error %d: %s\n", error, description);
// }

class GUI : public IView {
        AppModel *appModel;
        void showMainView();
        void showMainMenu();
        void showVideoView();
        void showCreateAccount(AppModel* appModel, bool* p_open);
        void showLogin(AppModel* appModel, bool* p_open);
        void showResetPassword(AppModel* appModel, bool* p_open);
        void showUserProfile(AppModel* appModel, bool* p_open);
        // Our states
        bool show_main_window = true;
        bool show_video_info_window = false;
        bool show_create_account = false;
        bool show_login = false;
        bool show_user_profile = false;
        int loginRetryTimes = 0;
        bool show_reset_password = false;
        bool show_account_deleted = false;
        bool logoutClicked;
        bool hasError = false;
        bool show_newpass_prompt = false;
        bool show_error_popup = false;
        std::string videoURL = "";
        bool deleteClicked = false;
        bool show_multi_compare = false;
        std::string errorInfo;


        float progress = 0.0;

        ImVec2 screenSize;

        ImFont* roboto_regular_font;
        ImFont* roboto_bold_font;
        ImFont* roboto_light_font;
        ImFont* roboto_lightsmall_font;
        ImFont* roboto_title_font;

        ImVec4 youtube_black;
        ImVec4 youtube_dark;
        ImVec4 youtube_semidark;
        ImVec4 youtube_red;
        ImVec4 youtube_red_semitrans;
        ImVec4 youtube_red_verytrans;
        ImVec4 youtube_light;
        ImVec4 youtube_light_gray;

        float menu_bar_height = 0;

    public:
        GUI(AppModel *appModel);
        ~GUI();
        void updateView(AppModel &appModel) override;

};

#endif 