#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "src/stb_image.h"
#include <stdio.h>
#include <iostream>

#include <cstring>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

#include <iostream>
#include "gui.h"
#include "src/iconsFontAwesome.h"

using namespace std;

GUI::GUI(AppModel *appModel) : appModel{appModel} {
    appModel->addListener(this);
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Senty Video Analyzer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window\n";
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    roboto_regular_font = io.Fonts->AddFontFromFileTTF("src/Roboto/Roboto-Regular.ttf", 25.0f);
    roboto_bold_font = io.Fonts->AddFontFromFileTTF("src/Roboto/Roboto-Bold.ttf", 25.0f);
    roboto_light_font = io.Fonts->AddFontFromFileTTF("src/Roboto/Roboto-Light.ttf", 25.0f);
    roboto_lightsmall_font = io.Fonts->AddFontFromFileTTF("src/Roboto/Roboto-Light.ttf", 16.0f);
    roboto_title_font = io.Fonts->AddFontFromFileTTF("src/Roboto/Roboto-Bold.ttf", 100.0f);
    roboto_title_font = io.Fonts->AddFontFromFileTTF("src/Roboto/Roboto-Bold.ttf", 25.0f);


    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 1.0f;
    style.FramePadding = ImVec2(12, 12);
    style.ItemSpacing = ImVec2(30, 30);
    style.FrameRounding = 3.0f; 
    ImGui::GetStyle().ItemSpacing = ImVec2(3, 20);
    menu_bar_height = ImGui::GetFrameHeightWithSpacing() ;
    youtube_black = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    youtube_dark = ImVec4(0.120f, 0.120f, 0.120f, 1.0f);
    youtube_semidark = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    youtube_red = ImVec4(0.877f, 0.000f, 0.000f, 1.0f);
    youtube_red_semitrans = ImVec4(0.877f, 0.000f, 0.000f, 0.6f);
    youtube_red_verytrans = ImVec4(0.877f, 0.000f, 0.000f, 0.3f);
    youtube_light = ImVec4(0.937f, 0.937f, 0.937f, 1.0f);
    youtube_light_gray = ImVec4(0.9000f, 0.831f, 0.831f, 1.0f);

    // Background
    style.Colors[ImGuiCol_WindowBg] = youtube_dark;

    // // Text
    style.Colors[ImGuiCol_Text] = youtube_light;
    style.Colors[ImGuiCol_TextDisabled] = youtube_light_gray;
    
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.000f, 0.000f, 0.000f, 1.00f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.0f);
    style.Colors[ImGuiCol_FrameBg]               = youtube_semidark;
    style.Colors[ImGuiCol_FrameBgHovered]        = youtube_red_semitrans;
    style.Colors[ImGuiCol_FrameBgActive]         = youtube_red;
    style.Colors[ImGuiCol_TitleBg]               = youtube_dark;
    style.Colors[ImGuiCol_TitleBgCollapsed]      = youtube_semidark;
    style.Colors[ImGuiCol_TitleBgActive]         = youtube_dark;
    style.Colors[ImGuiCol_MenuBarBg]             = youtube_black;
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button]                = youtube_red_verytrans;
    style.Colors[ImGuiCol_ButtonHovered]         = youtube_red_semitrans;
    style.Colors[ImGuiCol_ButtonActive]          = youtube_red;
    style.Colors[ImGuiCol_Header]                = youtube_semidark;
    style.Colors[ImGuiCol_HeaderHovered]         = youtube_red_semitrans;
    style.Colors[ImGuiCol_HeaderActive]          = youtube_red;
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_PlotLines]             = youtube_red_semitrans;
    style.Colors[ImGuiCol_PlotLinesHovered]      = youtube_red;
    style.Colors[ImGuiCol_PlotHistogram]         = youtube_red_semitrans;
    style.Colors[ImGuiCol_PlotHistogramHovered]  = youtube_red;
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImVec2 screenSize = io.DisplaySize;
        screenSize = io.DisplaySize;

        // ImGui::SetNextWindowSize(ImVec2(500, 500));
        ImGui::SetNextWindowPos(ImVec2(0, menu_bar_height));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x, screenSize.y-menu_bar_height));

        // center display and also size
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        showMainView();

        // Rendering
        ImGui::Render();
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }   
    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

GUI::~GUI() {}

void GUI::updateView(AppModel &appModel) {
    std::cout << "appModel.progress = " << appModel.getProgress() << std::endl;
    progress = appModel.getProgress() / 100.0;
    std::cout << "progress = " << progress << std::endl;
}

void GUI::showMainView() {
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    if (show_main_window) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(100, 100)); // Set the padding to 20 pixels on both X and Y axes
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(25, 25));

        ImGui::SetNextWindowPos(ImVec2(0, menu_bar_height));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x, screenSize.y - menu_bar_height));

        ImGui::Begin("Link Processor", nullptr, window_flags);
        
        showVideoView();
        

        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 button_size(50, 50); // Adjust the size of the help button

        ImGui::SetCursorPos(ImVec2(window_size.x - button_size.x - 50, 20));

        if (ImGui::Button("?", button_size)) {}

        // Show a tooltip when hovering over the help button
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20)); // Set the padding to 10 pixels on both X and Y axes
        ImGui::PushFont(roboto_lightsmall_font);
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Welcome to Senty, a YouTube sentiment analyzer.");
            ImGui::Text("We fetch comments and video information from YouTube's public API");
            ImGui::Text("and process that information so that you can know more about a video.");
            ImGui::NewLine();

            ImGui::Text("To get an analysis:");
            ImGui::BulletText("Input a link to a video (has to be the full link)");
            ImGui::Text("It might take a few seconds for the data to process,");
            ImGui::Text("especially for videos with larger comments sections.");
            ImGui::NewLine();

            ImGui::Text("Accounts:");
            ImGui::BulletText("You have 3 tries to log into your account.");
            ImGui::BulletText("You can reset your password if you've forgotten it,");
            ImGui::Text("as long as you know your security answer!");
            ImGui::EndTooltip();
        }
        ImGui::PopFont();
        ImGui::PopStyleVar();

        ImGui::End();
        showMainMenu();

        if (show_login) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, youtube_black);
            showLogin(appModel, &show_login);
            ImGui::PopStyleColor();
        }
        if (show_create_account) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, youtube_black);
            showCreateAccount(appModel, &show_create_account);
            ImGui::PopStyleColor();

        }
        if (show_reset_password) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, youtube_black);
            showResetPassword(appModel, &show_reset_password);
            ImGui::PopStyleColor();
        }
        if (show_user_profile) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, youtube_black);
            showUserProfile(appModel, &show_user_profile);
            ImGui::PopStyleColor();
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    }

}

void GUI::showMainMenu() {
    ImGui::BeginMainMenuBar();
    std::shared_ptr<Account> currentUser = appModel->getCurrentUser();
    if (currentUser == nullptr) { // not logged in
        if (ImGui::MenuItem("Create Account", NULL, &show_create_account)) {
            std::cout <<"create account button toggled\n" << std::endl;
            show_create_account = true;
            show_reset_password = false;
            show_login = false;
        }
        if (ImGui::MenuItem("Login", NULL, &show_login)) {
            std::cout <<"login button toggled\n" << std::endl;
            show_login = true;
            show_create_account = false;
            show_reset_password = false;
        }


    } else {
        // logout
        if (ImGui::MenuItem("Log Out", NULL, &logoutClicked)) {
            show_main_window = appModel->logout();
        }
        if (ImGui::MenuItem("Delete Account", NULL, &deleteClicked)) {
            // show_delete = true;
            ImGui::OpenPopup("Delete Account Confirmation");
        }
        if (ImGui::MenuItem("Account Info", NULL, &show_user_profile)) {
            show_user_profile = true;
        }
        // Modal Popup for Delete Account Confirmation
        if (ImGui::BeginPopupModal("Delete Account Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Are you sure you want to delete your account?");

    if (ImGui::Button("Yes")) {
        std::shared_ptr<Account> currentUser = appModel->getCurrentUser();
        AccountManager aManager = appModel->getAccountManager();
        std::string username = currentUser->getUsername();
        // Perform the account deletion process here
        appModel->deleteAccount(username);

        // Show the "Account deleted" popup
        show_account_deleted = true;

        // Close the confirmation popup
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();

    if (ImGui::Button("Cancel")) {
        // Close the popup without performing any action
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

// Add a new popup for the "Account deleted" message
if (show_account_deleted) {
    ImGui::OpenPopup("Account Deleted");
}

if (ImGui::BeginPopupModal("Account Deleted", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Account deleted.");

    if (ImGui::Button("OK")) {
        // Close the popup and reset the show_account_deleted flag
        show_account_deleted = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

    }
    // reset password for both logged in and not logged in
    if (ImGui::MenuItem("Reset Password", NULL, &show_reset_password)) {
        std::cout <<"reset password button toggled\n" << std::endl;
        show_reset_password = true;
        std::cout << show_reset_password << std::endl;
        show_create_account = false;
        show_login = false;
        
        
    }
    ImGui::EndMainMenuBar();

}

void GUI::showCreateAccount(AppModel* appModel, bool* p_open) {
    if (*p_open) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x, screenSize.y));
        static char username[128];
        static char password[128];
        static char security_question[128];
        static char security_answer[128];

        ImGui::Begin("Create Account", p_open, window_flags);
        ImGui::PushFont(roboto_title_font);
        ImGui::Text("Create Account");
        ImGui::PopFont();
        ImGui::InputText("Username", username, 128);
        ImGui::InputText("Password", password, 128, ImGuiInputTextFlags_Password);
        ImGui::InputText("Security Question", security_question, 128);
        ImGui::InputText("Security Answer", security_answer, 128);

        if (ImGui::Button("Create Account")) {
            // Implement create account logic here, using the AppModel and AccountManager
            try {
                AccountManager aManager = appModel->getAccountManager();
                appModel->createAccount(username, password, security_question, security_answer);
                // aManager.createAccount(username, password, security_question, security_answer);
                std::cout << "account created!\n" << std::endl;
                show_create_account = false;
                show_main_window = true;
                hasError = false;
                errorInfo = "";
            } catch (AccountaManagerException &e) {
                hasError = true;
                errorInfo = e.what();
            }
        }
        ImGui::SameLine(0.0f, 20.0f);
        if (ImGui::Button("Cancel")) {
            show_create_account = false;
            show_main_window = true;
            username[0] = '\0';
            password[0] = '\0';
            security_question[0] = '\0';
            security_answer[0] = '\0';
            hasError = false;
            errorInfo = "";
        }
        if (hasError) {
            show_error_popup = true;
            security_question[0] = '\0';
            username[0] = '\0';
            password[0] = '\0';
            security_answer[0] = '\0';
        }
        if (show_error_popup) {
            ImGui::OpenPopup("Error Popup");
        }
        if (ImGui::BeginPopupModal("Error Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", errorInfo.c_str());

            if (ImGui::Button("OK")) {
                // Close the popup and reset the show_error_popup flag
                show_error_popup = false;
                hasError = false;
                errorInfo = "";
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::End();
    }
}


void GUI::showLogin(AppModel* appModel, bool* p_open) {
    
    if (*p_open) {
        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x, screenSize.y));
        static char username[128];
        static char password[128];
        // static int tries = 0;
        {
        hasError = false;
        errorInfo = "";
        // std::cout << "showing login" << std::endl;
        ImGui::Begin("Login", p_open, window_flags);

        ImGui::PushFont(roboto_title_font);
        ImGui::Text("Login");
        ImGui::PopFont();

        ImGui::InputText("Username", username, 128);
        ImGui::InputText("Password", password, 128, ImGuiInputTextFlags_Password);

        if (ImGui::Button("Login")) {
            try {
                appModel->login(username, password);
                std::cout << "logged in successfully" << std::endl;
                showMainMenu();
                show_login = false;
                loginRetryTimes = 0;
                show_main_window = true;
            } catch(LoginException &e) {
                loginRetryTimes ++;
            }
        }
        ImGui::SameLine(0.0f, 20.0f);
        if (ImGui::Button("Cancel")) {
            show_login = false;
            show_main_window = true;
            username[0] = '\0';
            password[0] = '\0';
            loginRetryTimes = 0;
        }

        if (loginRetryTimes >= 3) {
            ImGui::Text("Account locked. Reset your password.");
            if (ImGui::Button("Reset password")) {
                show_login = false;
                loginRetryTimes = 0;
                show_main_window = true;
            }
        } else if (loginRetryTimes > 0) {
            ImGui::Text("Incorrect username or password, please try again.");

            ImVec4 linkColor = youtube_red_semitrans; // blue color
            ImVec4 linkHoverColor = youtube_red; // blue color with opacity

            ImGui::TextColored(linkColor, "Forgot your password?");
            if (ImGui::IsItemHovered()) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                ImGui::TextColored(linkHoverColor, "reset it now");
                if (ImGui::IsMouseClicked(0)) {
                    show_reset_password = true;
                    show_login = false;
                }
            }

        }

        ImGui::End();
        }
    }
}


void GUI::showResetPassword(AppModel* appModel, bool* p_open) {
    if (*p_open) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x, screenSize.y));
        ImGui::Begin("Reset Password", p_open, window_flags);

        static char username[128];
        static char new_password[128];
        static char security_answer[128];

        ImGui::PushFont(roboto_title_font);
        ImGui::Text("Reset Password");
        ImGui::PopFont();

        static std::string securityQuestion = "";
        ImGui::InputText("Username", username, 128);
        if (ImGui::Button("Find user")) {
            securityQuestion = appModel->getUserSecurityQuestion(username); 
            if (securityQuestion == "") {
                hasError = true;
                errorInfo = "Please enter a valid username";
            } else {
                hasError = false;
                errorInfo = "";
                show_newpass_prompt = true;
            }
        }
        if (show_newpass_prompt) {
            AccountManager aManager = appModel->getAccountManager(); 
            ImGui::Text("%s", securityQuestion.c_str());
            ImGui::InputText("Security Answer", security_answer, 128, ImGuiInputTextFlags_Password);
            ImGui::InputText("New Password", new_password, 128, ImGuiInputTextFlags_Password);

            if (ImGui::Button("Reset Password")) {
                try {
                    appModel->resetUserPassword(username, new_password, securityQuestion, security_answer);
                    std::cout << "reset successfully!" << std::endl;
                    show_reset_password = false;
                    show_main_window = true;
                    securityQuestion = "";
                } catch (AccountaManagerException &e) {
                    hasError = true;
                    errorInfo = e.what();
                }
            }
        }
        ImGui::SameLine(0.0f, 20.0f);
        if (ImGui::Button("Cancel")) {
            show_reset_password = false;
            show_main_window = true;
            show_newpass_prompt = false;
            username[0] = '\0';
            new_password[0] = '\0';
            security_answer[0] = '\0';
            securityQuestion = "";
            hasError = false;
            errorInfo = "";
        }
        if (hasError) {
            ImGui::Text("%s", errorInfo.c_str());
        }

        ImGui::End();
    }
}

void GUI::showUserProfile(AppModel* appModel, bool* p_open) {
    if (*p_open) {
        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x, screenSize.y));
        ImGui::Begin("User Profile", p_open, window_flags);

        std::shared_ptr<Account> currentUser = appModel->getCurrentUser();
        if (currentUser != nullptr) {
            hasError = false;
            errorInfo = "";
            ImGui::Text("Username: %s", currentUser->getUsername().c_str());

            // Display account history
            ImGui::Text("Account History: ");
            std::vector<std::string> history = currentUser->getHistory();


            ImVec4 linkColor = youtube_red_semitrans;
            // ImVec4 linkHoverColor = youtube_red; 

            for (const auto& url : history) {
                // ImGui::Text("%s", url.c_str());
                ImGui::TextColored(linkColor, "%s", url.c_str());
                if (ImGui::IsItemHovered()) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                    if (ImGui::IsMouseClicked(0)) {
                        videoURL = url;
                        show_user_profile = false;
                        show_main_window = true;
                    }
                }

            }
        }
        
        if (ImGui::Button("Go Home")) {
            show_user_profile = false;
            show_main_window = true;
        }

        ImGui::End();
    }
}

void GUI::showVideoView() {
    vector<Video> allVids;
    static char link[256];
    static char link2[256];
    static char title[256];
    static char title2[256];
    static char publishedAt[32];
    static char vidID[128];
    static char channelTitle[256];
    static char description[1024];
    static char tags[1024];
    static char channelId[128];
    static char duration[32];
    static char viewCount[64];
    static char likeCount[64];
    static char privacyStatus[32];
    static char caption[16];

    static char videoAnalysis[256];

    static char commentAnalysis[128];
    static char numComments[64];
    static char suggestions[2048];
    static char recommended[2048];

    static char likeViewCompare[2048];
    static char commentViewCompare[2048];
    static char sentimentCompare[2048];

        ImGui::PushFont(roboto_title_font);

        ImTextureID myImage = ImGui::GetIO().Fonts->TexID;
        int width, height;
        unsigned char* pixels = stbi_load("src/yt-logo.png", &width, &height, nullptr, 4);

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

    
        // myImage = (ImTextureID) textureID;
        myImage = reinterpret_cast<ImTextureID>(static_cast<intptr_t>(textureID));

        ImGui::Image(myImage, ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 0));

        stbi_image_free(pixels);

        ImGui::Text("Senty: Youtube Analyzer");
        ImGui::PopFont();
        if (videoURL != "") {
            strcpy(link, videoURL.c_str());
            videoURL = "";
        }
        // bool capture_multi_click = false;
        ImGui::Checkbox("Compare 2 videos", &show_multi_compare);
        // if (ImGui::IsMouseClicked(0)) {

        ImGui::InputText("Link1", link, 128);

        if (show_multi_compare) {
            ImGui::InputText("Link2", link2, 128);
        }
        ImGui::PushFont(roboto_bold_font);

        if (ImGui::Button(show_multi_compare?"Compare":"Process")) {
            if (show_multi_compare) {
            try {
                VideoManager manager = *appModel->getVideoManager();
                if (strcmp(link,link2) == 0) {
                    hasError = true;
                    errorInfo = "Both URLs are the same.";
                } else {
                    manager.addVideo(link);
                    manager.addVideo(link2);
                    allVids = manager.getVideos();
                    std::shared_ptr<Account> currentUser = appModel->getCurrentUser();
                    if (currentUser != nullptr) {
                        AccountManager aManager = appModel->getAccountManager();
                        aManager.addVideoHistory(link, *currentUser);
                        aManager.addVideoHistory(link2, *currentUser);
                    }

                    manager.setLikeViewCompare(0, 1);
                    manager.setCommentViewCompare(0, 1);
                    manager.setSentimentCompare(0, 1);
                    snprintf(title, sizeof(title), "%s", allVids[0].getTitle().c_str());
                    snprintf(title2, sizeof(title2), "%s", allVids[1].getTitle().c_str());
                    snprintf(likeViewCompare, sizeof(likeViewCompare), "%s", manager.getLikeViewCompare().c_str());
                    std::cout << manager.getLikeViewCompare() << std::endl;
                    snprintf(commentViewCompare, sizeof(commentViewCompare), "%s",  manager.getCommentViewCompare().c_str());
                    snprintf(sentimentCompare, sizeof(sentimentCompare), "%s", manager.getSentimentCompare().c_str());
                    
                    show_video_info_window = true;
                }
                
            } catch (VideoException &e) {
                errorInfo = e.what();
                hasError = true;
            } catch (RequestException &e) {
                errorInfo = e.what();
                hasError = true;
            }
        

            } else {
        try {
            VideoManager manager = *appModel->getVideoManager();
            manager.addVideo(link);
            allVids = manager.getVideos();
            std::shared_ptr<Account> currentUser = appModel->getCurrentUser();
            if (currentUser != nullptr) {
                AccountManager aManager = appModel->getAccountManager();
                aManager.addVideoHistory(link, *currentUser);
            }
            

            // title = "";
            for (const auto & vid : allVids) {
                // title += vid.getTitle();
                CommentSect commentsection = vid.getCommentSect();
                snprintf(title, sizeof(title), "%s", vid.getTitle().c_str());
                snprintf(publishedAt, sizeof(publishedAt), "%s", vid.getPublishedAt().c_str());
                snprintf(channelId, sizeof(channelId), "%s", vid.getChannelId().c_str());
                snprintf(vidID, sizeof(vidID), "%s", vid.getVidID().c_str());
                snprintf(description, sizeof(description), "%s", vid.getDescription().c_str());
                // for (const auto & tag : tags) {
                snprintf(tags, sizeof(tags), "%s", vid.getTags().c_str());
                // }
                snprintf(channelTitle, sizeof(channelTitle), "%s", vid.getChannelTitle().c_str());
                snprintf(duration, sizeof(duration), "%s", vid.getDuration().c_str());
                snprintf(caption, sizeof(caption), "%s", vid.getCaption().c_str());

                snprintf(viewCount, sizeof(viewCount), "%i", vid.getViewCount());
                snprintf(likeCount, sizeof(likeCount), "%i", vid.getLikeCount());

                snprintf(privacyStatus, sizeof(privacyStatus), "%s", vid.getPrivacyStatus().c_str());

                snprintf(videoAnalysis, sizeof(videoAnalysis), "%s", vid.getRatioAnalysis().c_str());
                snprintf(commentAnalysis, sizeof(commentAnalysis), "%s", vid.getCommentSect().getCommentSectAnalysis().c_str());
                snprintf(numComments, sizeof(numComments), "%i", vid.getCommentSect().getNumComments());
                snprintf(suggestions, sizeof(suggestions), "%s", vid.getSuggestionAnalysis().c_str());
                snprintf(recommended, sizeof(recommended), "%s", vid.getRecommended().c_str());
            }
            show_video_info_window = true;
            
        } catch (VideoException &e) {

            errorInfo = e.what();
            hasError = true;
        } catch (RequestException &e) {
            errorInfo = e.what();
            hasError = true;
        }
        }
        }
        if (hasError) {
            ImGui::Text("%s", errorInfo.c_str());
            link[0] = '\0';
            link2[0] = '\0';
        }

        ImGui::PopFont();
    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse ;

    if (show_video_info_window) {
        ImGui::PushFont(roboto_light_font);

        ImGui::SetNextWindowPos(ImVec2(0, menu_bar_height));
        ImGui::SetNextWindowSize(ImVec2(screenSize.x, screenSize.y - menu_bar_height));
        // show_main_window = false;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Video Window", &show_video_info_window, window_flags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when button toggled)
        
        hasError = false;
        errorInfo = "";

        if (!show_multi_compare) {
            ImGui::PushFont(roboto_title_font);
            ImGui::Text("Sentiment Analysis");
            ImGui::PopFont();
            {
                ImGui::SeparatorText("Video Info");
                ImGui::BeginGroup();
                    // Display title
                    ImGui::Text("Title: %s", title);
                    // Display published at
                    ImGui::Text("Published at: %s", publishedAt);
                    // Display channel id
                    ImGui::Text("Channel id: %s", channelId);
                    // Display video id
                    ImGui::Text("Video id: %s", vidID);
                    // Display description
                    ImGui::Text("Description: %s", description);
                    // Display tags
                    ImGui::Text("Tags: %s", tags);
                    // Display channel title
                    ImGui::Text("Channel title: %s", channelTitle);
                    // Display duration
                    ImGui::Text("Duration: %s", duration);
                    // Display caption
                    ImGui::Text("Caption: %s", caption);
                    // Display view count
                    ImGui::Text("View count: %s", viewCount);
                    // Display like count
                    ImGui::Text("Like count: %s", likeCount);
                    // Display privacy status
                    ImGui::Text("Privacy status: %s", privacyStatus);
                ImGui::EndGroup();
                
            }
            ImGui::SeparatorText("Video Section Analysis");
            {
                ImGui::BeginGroup();
                    ImGui::Text("Video Engagement");
                    ImGui::Text("%s", videoAnalysis);
                    ImGui::Text("Suggestions: %s\n", suggestions);
                    ImGui::Text("Recommendations: %s\n", recommended);
                ImGui::EndGroup();
            }

            ImGui::SeparatorText("Comment Section Analysis");
            {
                ImGui::Text("Comment Section Analysis");
                ImGui::BeginGroup();
                    ImGui::Text("# Comments: %s", numComments);
                    ImGui::Text("CommentAnalysis Info");
                    ImGui::Text("Comment Analysis: %s", commentAnalysis);

                ImGui::EndGroup();

            }
        } else {
            {
                ImGui::BeginGroup();
                    ImGui::Text("Video 1: %s", title);
                    ImGui::Text("Video 2: %s", title2);
                    ImGui::Text("CommentAnalysis Info");
                    ImGui::Text("Comment:View Comparison:\n%s", commentViewCompare);
                    ImGui::Text("Sentiment Comparison: \n%s", sentimentCompare);
                    ImGui::Text("Like:View Comparison: \n%s", likeViewCompare);
                ImGui::EndGroup();
            }
        }

        {
            if (ImGui::Button("Go Back to Home")) {
                show_video_info_window = false;
                show_main_window = true;
                link[0] = '\0';
                link2[0] = '\0';
            }
        }
        ImGui::PopFont();
        ImGui::End();
    }
    
}

