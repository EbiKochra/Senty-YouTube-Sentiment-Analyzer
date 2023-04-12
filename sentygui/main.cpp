#include "appModel.h"
#include "gui.h"


// int main() {
int main(int, char**) {
	AppModel *appModel = new AppModel();
    new GUI(appModel);

    return 0;
}
