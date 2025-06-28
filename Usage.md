## Reproducing Results

To reproduce the results of our experiments, please follow the steps below. If you have any problems, check the troubleshooting tips at the end of this section.

### 1. Build Dependencies

- **Xerus Library:**  
 Download and build the [Xerus library](https://libxerus.org/building_xerus/) by following their official guide. I recommend using the branch `johannas-master`.

- **Git LFS:**  
 Make sure [Git Large File Storage (LFS)](https://git-lfs.com/) is installed and initialized:
 ```bash
  git lfs install
 ```

- **Matio Library:**  
 Install the matio library. For Ubuntu:
 ```bash
  sudo apt install libmatio-dev
 ```
 For other platforms, refer to [matio's documentation](https://github.com/tbeu/matio).

### 2. Clone the Repository

Clone this repository using Git:
```bash
git clone https://github.com/jbellloni/xerus-compression-example.git
cd xerus-compression-example
```

### 3. Compile the Code

Compile the main experiment file (`Miranda.cpp`):
```bash
gcc Miranda.cpp -o Miranda.o -fsanitize=address -lmatio -lstdc++ -lm -lxerus -lxerus_misc
```
Make sure the required libraries are discoverable by your compiler (adjust the `-I` and `-L` flags if needed).

### 4. Run the Experiment

Execute the compiled binary:
```bash
./Miranda.o
```

### Notes & Troubleshooting

- **Memory Requirements:**  
 The computations require significant memory. Ensure your system has enough RAM to avoid unexpected termination.

- **Running on WSL:**  
 If you are using Windows Subsystem for Linux (WSL), you may need to increase the available memory. Edit your `wsl.config` to allocate more resources. Refer to [WSL documentation](https://learn.microsoft.com/en-us/windows/wsl/wsl-config) for details.

- **Additional Help:**  
 If you encounter build or runtime errors, please check the documentation of the respective libraries or open an issue in this repository.