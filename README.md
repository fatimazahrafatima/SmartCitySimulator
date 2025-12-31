# Practice Project: Mastering Git and GitHub Workflow

Welcome to your first collaborative project! This simple C++ application uses the Raylib library and the CMake build system.

The primary goal of this exercise is **not** the C++ code itself, but to master the professional development workflow: **Forking, Cloning, Branching, Committing, and creating Pull Requests (PRs).**

Mastering this workflow now will ensure our main project runs smoothly and professionally.

---

## Part 1: Initial Setup and Running the Project

Before you write a single line of code, you must set up your development environment and successfully compile and run the base project.

### Environment A: Windows Setup (Using MSVC Compiler)

#### Prerequisites (What to Install on Windows)

You need to install three essential tools. **Follow the instructions exactly.**

1.  **Git Bash:** This provides the professional Linux-style terminal commands needed for all your Git operations.
    *   **Installation:** Download and run the installer from [git-scm.com](https://git-scm.com/download/win). Use all the **default installation options**.
    *   *Verification:* Open the **Git Bash** application and type `git --version`. Ensure a version number is displayed.

2.  **Visual Studio Build Tools (The C++ Compiler - MSVC):**
    *   **Installation:** Download the **Visual Studio Installer** from the official Microsoft site.
    *   **CRUCIAL STEP:** When prompted for workloads, **SELECT ONLY** the **"Desktop development with C++"** workload.
    *   *Verification:* Search Windows Start Menu for **"x64 Native Tools Command Prompt"**. Open it and type `cl`.

3.  **CMake:**
    *   **Installation:** Download from [cmake.org/download](https://cmake.org/download/).
    *   **CRUCIAL STEP:** Select the option: **"Add CMake to the system PATH for all users."**
    *   *Verification:* Open **Git Bash** and type `cmake --version`.

#### **Setup Step: Configure Git Credentials (Windows)**

We will use **Git Credential Manager (GCM)**. This allows you to sign in via a browser window once, and it will remember your login so you don't have to type your password every time you push code.

1.  Open **Git Bash**.
2.  Configure your username and email (this attaches your name to your code history):
    ```bash
    git config --global user.name "Your Actual Name"
    git config --global user.email "your_email@example.com"
    ```
3.  Enable the Credential Manager:
    ```bash
    git config --global credential.helper manager
    ```
    *Note: The first time you try to clone or push to a repository later in this guide, a pop-up window will appear. Select **"Sign in with your browser"** and authorize GitHub.*

---

#### VS Code and CMake Tools Setup (Windows)

1.  **Install VS Code:** Download and install Visual Studio Code.
2.  **Install the CMake Tools Extension:** In VS Code, go to Extensions (`Ctrl+Shift+X`) and install **"CMake Tools"** by Microsoft.

#### Build Steps: Running the Existing Code (Windows)

1.  **Clone the Repository (Use Git Bash):**
    ```bash
    cd ~/Documents 
    git clone https://github.com/lst-cpp-project-2025-2026/practice-project.git
    cd practice-project
    ```

2.  **Configure and Build (Use the "x64 Native Tools Command Prompt"):**
    *   Open the **"x64 Native Tools Command Prompt"** from the Start Menu.
    *   Navigate and build:
    ```batch
    cd C:\Users\YourName\Documents\practice-project 
    mkdir build
    cd build
    cmake .. 
    cmake --build .
    .\Debug\RaylibPracticeProject.exe 
    ```
    *   A Raylib window should appear. **Success!**

3.  **VS Code Configuration:**
    *   Open the `practice-project` folder in VS Code.
    *   In the status bar (bottom), click the Kit selector.
    *   **Select the MSVC kit** (e.g., `Visual Studio Community 2022 Release - amd64`). **Do NOT select MinGW/GCC.**

---

### Environment B: Ubuntu/Linux Setup (Using GCC Compiler)

#### Prerequisites (What to Install on Ubuntu)

1.  **System Update & Essentials:**
    ```bash
    sudo apt update
    sudo apt install git cmake build-essential
    ```
2.  **Raylib Dependencies:**
    ```bash
    sudo apt install libx11-dev libgl1-mesa-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libwayland-dev libasound-dev
    ```

#### **Setup Step: Configure SSH Credentials (Ubuntu)**

Linux users use **SSH keys** to authenticate. This allows you to push code without a password by matching a key file on your computer with one on GitHub.

1.  **Generate an SSH Key:**
    Open your terminal and run the following. Press **Enter** for the file location (default) and press **Enter** twice to leave the passphrase empty (this ensures you don't have to type a password every time).
    ```bash
    ssh-keygen -t ed25519 -C "your_email@example.com"
    ```

2.  **Copy Your Public Key:**
    Display the key you just generated:
    ```bash
    cat ~/.ssh/id_ed25519.pub
    ```
    *Highlight and copy the entire string of text output (starting with `ssh-ed25519` and ending with your email).*

3.  **Add Key to GitHub:**
    *   Go to GitHub.com > **Settings** (Click your profile picture in top right).
    *   On the left sidebar, click **SSH and GPG keys**.
    *   Click the green **New SSH key** button.
    *   **Title:** "Ubuntu Laptop" (or similar).
    *   **Key:** Paste the text you copied in step 2.
    *   Click **Add SSH key**.

4.  **Verify Connection:**
    Back in your terminal, type:
    ```bash
    ssh -T git@github.com
    ```
    *Type `yes` if asked to continue connecting. You should see: "Hi [Username]! You've successfully authenticated..."*

5.  **Configure Git Identity:**
    ```bash
    git config --global user.name "Your Actual Name"
    git config --global user.email "your_email@example.com"
    ```

---

#### Build Steps: Running the Existing Code (Ubuntu Terminal)

1.  **Clone the Repository:**
    *Note: Since we just set up SSH, we will use the SSH link format.*
    ```bash
    cd ~/Documents 
    git clone git@github.com:lst-cpp-project-2025-2026/practice-project.git
    cd practice-project
    ```

2.  **Configure and Build:**
    ```bash
    mkdir build
    cd build
    cmake .. 
    cmake --build .
    ./RaylibPracticeProject
    ```

3.  **VS Code Configuration:**
    *   Open folder in VS Code.
    *   CMake Tools will detect `GCC x86_64-linux-gnu`. This is correct.

---

## Part 2: The Contribution Workflow (Your Task)

This section details the professional Git/GitHub workflow.

### Your Task

Your goal is to find an open issue, add a simple colored box containing your name to the window in `src/main.cpp`, and submit a Pull Request (PR).

### Step-by-Step GitHub Workflow

#### **Step 1: Get the Assignment (On GitHub)**

1.  Go to the **Issues** tab on the original repository.
2.  Comment "I'd like to take this!" on an open issue.
3.  Wait to be assigned.

#### **Step 2: Fork the Repository (On GitHub)**

1.  Go to the main repository page on GitHub.
2.  Click the **Fork** button (top right).
3.  Create the fork under your own account.

#### **Step 3: Clone Your Fork Locally (In Terminal/Git Bash)**

***Delete the original `practice-project` folder from Part 1 before doing this.***

1.  Go to **your personal fork** on GitHub.
2.  Click the **Code** button.
    *   **WINDOWS Users:** Ensure the **HTTPS** tab is selected. Copy the URL.
    *   **UBUNTU Users:** Ensure the **SSH** tab is selected. Copy the URL (starts with `git@...`).
3.  In your terminal, clone your fork:
    ```bash
    git clone <your-fork-url>
    cd practice-project
    ```
4.  **Re-configure Build:** Run `mkdir build`, `cd build`, and `cmake ..` again as per Part 1 instructions for your OS.

#### **Step 4: Link to Upstream**

Link your local repo to the original project to keep it updated.

*   **Windows Users (HTTPS):**
    ```bash
    git remote add upstream https://github.com/lst-cpp-project-2025-2026/practice-project.git
    ```
*   **Ubuntu Users (SSH):**
    ```bash
    git remote add upstream git@github.com:lst-cpp-project-2025-2026/practice-project.git
    ```

#### **Step 5: Create a Feature Branch**

1.  Update master:
    ```bash
    git checkout master
    git pull upstream master
    git push origin master
    ```
2.  Create your branch:
    ```bash
    git checkout -b feature/add-your-name 
    ```

#### **Step 6: Code the Feature (In VS Code)**

1.  Edit `src/main.cpp` to add your box and name.
2.  **Test your code** by building and running it to ensure the box appears.

#### **Step 7: Commit and Push**

1.  Stage and Commit:
    ```bash
    git add src/main.cpp 
    git commit -m "feat: Add colored box for [Name] and closes #XX" 
    ```
2.  Push to GitHub:
    ```bash
    git push origin feature/add-your-name
    ```
    *   **Windows Users:** If this is your first time, a popup will appear. Log in with your browser.
    *   **Ubuntu Users:** This should push immediately without asking for a password.

#### **Step 8: Create the Pull Request (PR)**

1.  Go to **your fork** on GitHub.
2.  Click **"Compare & pull request."**
3.  Ensure **base repository** is the original project and **head repository** is your fork.
4.  Click **"Create pull request."**

#### **Step 9: Review and Merge**

Wait for the lead to review your code. If changes are requested, edit your code, commit, and push again—the PR will update automatically.
