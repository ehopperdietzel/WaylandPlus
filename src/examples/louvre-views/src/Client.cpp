#include <LLog.h>
#include <unistd.h>
#include <string.h>
#include "Client.h"
#include "App.h"
#include "Global.h"

static int get_ppid_from_proc(int pid)
{
    char filename[128];
    snprintf(filename, sizeof(filename), "/proc/%d/status", pid);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    int ppid = -1;
    char line[128];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "PPid:", 5) == 0) {
            sscanf(line + 6, "%d", &ppid);
            break;
        }
    }

    fclose(file);
    return ppid;
}

int get_process_name_by_pid(int pid, char *process_name, size_t buffer_size) {
    char pid_path[1024];
    ssize_t len;

    // Construct the path to the /proc/PID/exe symbolic link
    snprintf(pid_path, sizeof(pid_path), "/proc/%d/exe", pid);

    // Read the symbolic link to get the process name
    len = readlink(pid_path, process_name, buffer_size - 1);
    if (len == -1) {
        perror("readlink");
        return -1; // Error occurred
    }

    process_name[len] = '\0'; // Null-terminate the process name

    // Extract only the actual process name (strip the path)
    char *name = strrchr(process_name, '/');
    if (name != NULL) {
        name++; // Move past the last '/'
    } else {
        name = process_name; // Use the whole name if '/' is not found
    }

    strncpy(process_name, name, buffer_size); // Copy the process name to the provided buffer
    return 0; // Success
}

Client::Client(Params *params) : LClient(params)
{
    wl_client_get_credentials(client(), &pid, NULL, NULL);

    // Compositor pid
    Int32 cpid = getpid();
    Int32 ppid = pid;

    // Search the AppDock item that started it (if any)
    while (ppid != 1 && ppid != cpid)
    {
        for (App *app : G::apps())
        {
            if (app->pid == ppid)
            {
                if (!app->client)
                {
                    this->app = app;
                    app->client = this;
                    return;
                }
                else
                    return;
            }
        }

        ppid = get_ppid_from_proc(ppid);
    }
}

Client::~Client()
{
    if (app)
    {
        // Only destroy App if is not pinned to the dock
        if (app->pinned)
        {
            app->state = App::Dead;
            app->client = nullptr;
        }
        else
            delete app;
    }
}

void Client::createNonPinnedApp()
{
    if (app)
        return;

    char name[1024];
    get_process_name_by_pid(pid, name, sizeof(name));

    LLog::debug("Non pinned app name: %s", name);

    // If not a pinned Dock app, create a tmp AppDock item
    app = new App(name, NULL, NULL);
    app->client = this;
    app->pid = pid;
}