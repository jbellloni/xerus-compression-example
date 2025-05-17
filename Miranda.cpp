// compilation: gcc Miranda.cpp -o Miranda.o -fsanitize=address -lmatio -lstdc++ -lm -lxerus -lxerus_misc

#include <matio.h>
#include <iostream>
#include <xerus.h>
#include <vector>
#include <cmath> // required for std::sqrt, std::pow

using namespace xerus;

void save_to_mat(const std::string& filename, Tensor& tensor) {
    // Open the MAT file for writing
    mat_t* matFile = Mat_Create(filename.c_str(), NULL);
    if (!matFile) {
        std::cerr << "Error creating MAT file for writing!" << std::endl;
        return;
    }

    // Prepare data for writing
    const auto& dimensions = tensor.dimensions;  // Get tensor dimensions
    size_t dim0 = dimensions[0], dim1 = dimensions[1], dim2 = dimensions[2];
    size_t totalElements = dim0 * dim1 * dim2;

    std::unique_ptr<double[]> data(new double[totalElements]);
    std::memcpy(data.get(), tensor.get_dense_data(), totalElements * sizeof(double));  // Copy tensor data

    // Create the variable for the MAT file
    size_t dims[3] = {dim0, dim1, dim2};  // MATLAB uses size_t for dimensions
    matvar_t* matvar = Mat_VarCreate(
        "compressedTensor", MAT_C_DOUBLE, MAT_T_DOUBLE,
        3, dims, data.get(), 0
    );

    if (!matvar) {
        std::cerr << "Error creating variable in MAT file!" << std::endl;
        Mat_Close(matFile);
        return;
    }

    // Write the variable to the file
    if (Mat_VarWrite(matFile, matvar, MAT_COMPRESSION_NONE) != 0) {
        std::cerr << "Error writing variable to MAT file!" << std::endl;
    } else {
        std::cout << "Successfully wrote tensor to MAT file!" << std::endl;
    }

    // Clean up
    Mat_VarFree(matvar);
    Mat_Close(matFile);
}

int main() {

    Tensor mirandaTensor;
    TTTensor mirandaTTTensor;

    /////// Comment out the following block if you want to load the tensor from a file
    const char* filename = "density.mat";
    mat_t* matFile = Mat_Open(std::string("density.mat").c_str(), MAT_ACC_RDONLY);
    if (matFile == NULL) {
        std::cerr << "Error opening MAT file!" << std::endl;
        return 1;
    }

    std::cout << "Successfully loaded Miranda dataset with header: ";
    std::cout << Mat_GetHeader(matFile) << std::endl;
    
    std::cout << "Reading the only variable..." << std::endl;
    matvar_t* matvar = Mat_VarReadNext(matFile);
    std::cout << "Variable name: " << matvar->name << std::endl;
    std::cout << "Variable dimensions: ";
    size_t dim0 = matvar->dims[0], dim1 = matvar->dims[1], dim2 = matvar->dims[2];
    std::cout << dim0 << " x " << dim1 << " x " << dim2 << std::endl << std::endl;

    Tensor::DimensionTuple dimensions({dim0, dim1, dim2});

    std::cout << "Reading the data into a xerus::Tensor..." << std::endl;
    // Note, that using matio the data is already converted to row-major ordering
    if (matvar->data_type == MAT_T_DOUBLE) {
        std::unique_ptr<double[]> data(new double[dim0 * dim1 * dim2]);

        // Copy the data from the matvar to the tensor chunk-by-chunk
        // This is necessary to avoid memory issues with large datasets
        size_t chunkSize = 256 * 256;
        size_t nCopied = 0;
        double* source = static_cast<double*>(matvar->data);
        while (nCopied < dim0 * dim1 * dim2) {
            std::memcpy(data.get() + nCopied, source + nCopied, chunkSize * sizeof(double));
            nCopied += chunkSize;
        }
        Mat_Close(matFile);
        mirandaTensor = Tensor(dimensions, std::move(data));
    } else {
        std::cerr << "Unsupported data type!" << std::endl;
    }

    std::cout << "Successfully initialized the tensor!" << std::endl;
    std::cout << "Non-zero entries: " << mirandaTensor.count_non_zero_entries() << std::endl << std::endl;

    std::cout << "Saving the tensor to 'xerusTensors/MirandaTensor.dat'..." << std::endl;
    misc::save_to_file(mirandaTensor, "xerusTensors/MirandaTensor.dat");
    std::cout << "Successfully saved the tensor to 'xerusTensors/MirandaTensor.dat'!" << std::endl << std::endl;

    std::cout << "Computing the TT decomposition for the Miranda tensor..." << std::endl;
    mirandaTTTensor = TTTensor(mirandaTensor);
    std::cout << "Succesfully computed the TTTensor with ranks [";
    std::cout << mirandaTTTensor.ranks()[0] << ", " << mirandaTTTensor.ranks()[1] << "]" << std::endl << std::endl;
	
    std::cout << "Saving the TTTensor to 'xerusTensors/MirandaTTTensor.dat'..." << std::endl;
	misc::save_to_file(mirandaTTTensor, "xerusTensors/MirandaTTTensor.dat");
	std::cout << "Successfully saved the TTTensor to 'xerusTensors/MirandaTTTensor.dat'!" << std::endl << std::endl;
    /////// end commented block

    std::cout << "Loading the tensors from 'xerusTensors/*'..." << std::endl;
    mirandaTensor = misc::load_from_file<Tensor>("xerusTensors/MirandaTensor.dat");
    mirandaTTTensor = misc::load_from_file<TTTensor>("xerusTensors/MirandaTTTensor.dat");
    std::cout << "Successfully loaded the tensors!" << std::endl << std::endl;

    auto thresholds = {1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8};
    for(double maxError : thresholds) {
		TTTensor compressedTTTensor = TTTensor(mirandaTTTensor);
		std::cout << "Computing approximation for threshold error " << maxError << "..." << std::endl;
		compressedTTTensor.round(std::sqrt(maxError));
        xerus::Tensor compressedTensor = Tensor(compressedTTTensor);
		// and determine the squared relative approximation error
        double relError = std::pow(frob_norm(xerus::Tensor(compressedTensor)-mirandaTensor),2)
                                /std::pow(frob_norm(mirandaTensor),2);
        std::cout << "Ranks: [" << compressedTTTensor.ranks()[0] << ", " << compressedTTTensor.ranks()[1];
        std::cout << "], relative approximation error: " << relError << std::endl;
        std::cout << "Saving the compressed tensor to 'compressedTensor_" << maxError << ".mat'..." << std::endl;
        save_to_mat("compressionResults/compressedTensor_" + std::to_string(maxError) + ".mat", compressedTensor);
	}

    return 0;
}