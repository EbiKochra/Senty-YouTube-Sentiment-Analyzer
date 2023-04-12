class GUI {
    GUI() {
        while (!glfwWindowShouldClose(window)) {
            // Update ImGui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Call the updateView and showVideoView functions
            ImGui::Begin("Progress");
            // float progress = appModel.getProgress() / 100.0;
            ImGui::Text("Progress: %.2f%%", progress * 100.0f);
            ImGui::ProgressBar(progress, ImVec2(-1, 0), "");
            ImGui::End();

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Swap buffers and poll events
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    void updateView(AppModel &appModel) {
        progress = appModel.getProgress() / 100.0;
    }

}