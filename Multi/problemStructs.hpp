#include <ceres/ceres.h>
#include <ceres/rotation.h>


// Read a problem file specified in 'my' format
class SingleViewPoseAdjustmentProblem{

public:

	// Return the number of observations
	int getNumObs() const { return numObs_; }
	// Return the number of keypoints observed
	int getNumPts() const { return numPts_; }
	// Return the center of the car
	double* getCarCenter() { return carCenter_; }
	// Return the height of the car
	double getCarHeight() const { return h_; }
	// Return the width of the car
	double getCarWidth() const { return w_; }
	// Return the length of the car
	double getCarLength() const { return l_; }
	// Return a pointer to the observation vector
	double* observations() const { return observations_; }
	// Return a pointer to the observation weights vector
	double* observationWeights() const { return observationWeights_; }
	// Return a pointer to the camera intrinsics
	double* getK() const { return K_; }
	// Return a pointer to the mean 3D locations
	double* getX_bar() const { return X_bar_; }
	// Return a pointer to the top 5 eigenvectors
	double* getV() { return V_; }
	// Return a pointer to the weights (lambdas)
	double* getLambdas() { return lambdas_; }

	// Read data from input file
	bool loadFile(const char *fileName){
		FILE *fptr = fopen(fileName, "r");
		if(fptr == NULL){
			return false;
		}

		// numViews, numPts, numObs, numFaces
		fscanfOrDie(fptr, "%d", &numViews_);
		fscanfOrDie(fptr, "%d", &numPts_);
		fscanfOrDie(fptr, "%d", &numObs_);

		// Center of the car
		carCenter_ = new double[3];
		fscanfOrDie(fptr, "%lf", carCenter_+0);
		fscanfOrDie(fptr, "%lf", carCenter_+1);
		fscanfOrDie(fptr, "%lf", carCenter_+2);

		// Height, Width, and Length of the Car
		fscanfOrDie(fptr, "%lf", &h_);
		fscanfOrDie(fptr, "%lf", &w_);
		fscanfOrDie(fptr, "%lf", &l_);

		// Camera intrinsics
		K_ = new double[9*numViews_];
		for(int i = 0; i < 9*numViews_; ++i){
			fscanfOrDie(fptr, "%lf", K_ + i);
		}

		// Observations
		observations_ = new double[2*numObs_];
		for(int i = 0; i < numObs_; ++i){
			for(int j = 0; j < 2; ++j){
				fscanfOrDie(fptr, "%lf", observations_ + i*2 + j);
			}
		}

		// Observation weights
		observationWeights_ = new double[numObs_];
		for(int i = 0; i < numObs_; ++i){
			fscanfOrDie(fptr, "%lf", observationWeights_ + i);
		}

		// Mean locations
		X_bar_ = new double[3*numObs_];
		for(int i = 0; i < numObs_; ++i){
			for(int j = 0; j < 3; ++j){
				fscanfOrDie(fptr, "%lf", X_bar_ + i*3 + j);
			}
		}

		// Read in the top 5 eigenvectors for the shape
		// Size allocation: 5 vecs * 3 coordinates per vex * 14 keypoints (numPts_)
		V_ = new double[42*3*numPts_];
		for(int i = 0; i < 42; ++i){
			for(int j = 0; j < numPts_; ++j){
				fscanfOrDie(fptr, "%lf", V_ + i*3*numPts_ + 3*j + 0);
				fscanfOrDie(fptr, "%lf", V_ + i*3*numPts_ + 3*j + 1);
				fscanfOrDie(fptr, "%lf", V_ + i*3*numPts_ + 3*j + 2);
			}
		}

		// Read in the initial values for lambdas
		lambdas_ = new double[42];
		for(int i = 0; i < 42; ++i){
			fscanfOrDie(fptr, "%lf", lambdas_ + i);
		}



		// // Printing out data (for verification)
		// std::cout << "numViews: " << numViews_ << std::endl;
		// std::cout << "numPoints: " << numPts_ << std::endl;
		// std::cout << "numObs: " << numObs_ << std::endl;
		// std::cout << "K: " << K_[0] << " " << K_[1] << " " << K_[2] << " " << K_[3] << " " \
		// 	<< K_[4] << " " << K_[5] << " " << K_[6] << " " << K_[7] << " " << K_[8] << std::endl;
		// for(int i = 0; i < numObs_; ++i){
		// 	std::cout << "Obs: " << observations_[0+2*i] << " " << observations_[1+2*i] << std::endl;
		// }
		// for(int i = 0; i < numObs_; ++i){
		// 	std::cout << "ObsWeight: " << observationWeights_[i] << std::endl;
		// }
		// for(int i = 0; i < numObs_; ++i){
		// 	std::cout << "3D Point: " << X_bar_[0+3*i] << " " << X_bar_[1+3*i] << " " \
		// 	<< X_bar_[2+3*i] << std::endl;
		// }

		return true;

	}

private:

	// Helper function to read in one value to a text file
	template <typename T>
	void fscanfOrDie(FILE *fptr, const char *format, T *value){
		int numScanned = fscanf(fptr, format, value);
		if(numScanned != 1){
			LOG(FATAL) << "Invalid data file";
		}
	}

	// Private variables
	
	// Number of views
	int numViews_;
	// Number of keypoints
	int numPts_;
	// Number of observations
	int numObs_;

	// Center of the car
	double *carCenter_;
	// Dimensions of the car
	double h_, w_, l_;

	// Camera intrinsics
	double *K_;
	// Observation vector
	double *observations_;
	// Observation weight vector
	double *observationWeights_;
	// 3D point
	double *X_bar_;

	// Top 5 eigenvectors for the shape, i.e., the deformation basis vectors
	double *V_;

	// Weights for the eigenvectors
	double *lambdas_;

};




// Read a shape adjustment problem (a single view one)
class SingleViewShapeAdjustmentProblem{

public:

	// Return the number of observations
	int getNumObs() const { return numObs_; }
	// Return the number of keypoints observed
	int getNumPts() const { return numPts_; }
	// Return the center of the car
	double* getCarCenter() { return carCenter_; }
	// Return the height of the car
	double getCarHeight() const { return h_; }
	// Return the width of the car
	double getCarWidth() const { return w_; }
	// Return the length of the car
	double getCarLength() const { return l_; }
	// Return a pointer to the observation vector
	double* observations() const { return observations_; }
	// Return a pointer to the observation weights vector
	double* observationWeights() const { return observationWeights_; }
	// Return a pointer to the camera intrinsics
	double* getK() const { return K_; }
	// Return a pointer to the mean 3D locations
	double* getX_bar() const { return X_bar_; }
	// Return a pointer to the top 5 eigenvectors
	double* getV() { return V_; }
	// Return a pointer to the weights (lambdas)
	double* getLambdas() { return lambdas_; }
	// Return a pointer to the rotation estimated (from PnP)
	double* getRot() { return rot_; }
	// Return a pointer to the translation estimated (from PnP)
	double* getTrans() { return trans_; }

	// Read data from input file
	bool loadFile(const char *fileName){
		FILE *fptr = fopen(fileName, "r");
		if(fptr == NULL){
			return false;
		}

		// numViews, numPts, numObs, numFaces
		fscanfOrDie(fptr, "%d", &numViews_);
		fscanfOrDie(fptr, "%d", &numPts_);
		fscanfOrDie(fptr, "%d", &numObs_);

		// Center of the car
		carCenter_ = new double[3];
		fscanfOrDie(fptr, "%lf", carCenter_+0);
		fscanfOrDie(fptr, "%lf", carCenter_+1);
		fscanfOrDie(fptr, "%lf", carCenter_+2);

		// Height, Width, and Length of the Car
		fscanfOrDie(fptr, "%lf", &h_);
		fscanfOrDie(fptr, "%lf", &w_);
		fscanfOrDie(fptr, "%lf", &l_);

		// Camera intrinsics
		K_ = new double[9*numViews_];
		for(int i = 0; i < 9*numViews_; ++i){
			fscanfOrDie(fptr, "%lf", K_ + i);
		}
		
		// Observations
		observations_ = new double[2*numObs_];
		for(int i = 0; i < numObs_; ++i){
			for(int j = 0; j < 2; ++j){
				fscanfOrDie(fptr, "%lf", observations_ + i*2 + j);
			}
		}

		// Observation weights
		observationWeights_ = new double[numObs_];
		for(int i = 0; i < numObs_; ++i){
			fscanfOrDie(fptr, "%lf", observationWeights_ + i);
		}

		// Mean locations
		X_bar_ = new double[3*numObs_];
		for(int i = 0; i < numObs_; ++i){
			for(int j = 0; j < 3; ++j){
				fscanfOrDie(fptr, "%lf", X_bar_ + i*3 + j);
			}
		}

		// Read in the top 5 eigenvectors for the shape
		// Size allocation: 5 vecs * 3 coordinates per vex * 14 keypoints (numPts_)
		V_ = new double[42*3*numPts_];
		for(int i = 0; i < 42; ++i){
			for(int j = 0; j < numPts_; ++j){
				fscanfOrDie(fptr, "%lf", V_ + i*3*numPts_ + 3*j + 0);
				fscanfOrDie(fptr, "%lf", V_ + i*3*numPts_ + 3*j + 1);
				fscanfOrDie(fptr, "%lf", V_ + i*3*numPts_ + 3*j + 2);
			}
		}

		// Read in the initial values for lambdas
		lambdas_ = new double[42];
		for(int i = 0; i < 42; ++i){
			fscanfOrDie(fptr, "%lf", lambdas_ + i);
		}

		// Read in the rotation estimate (from PnP) (column-major ordered rotation matrix)
		rot_ = new double[9];
		for(int i = 0; i < 9; ++i){
			fscanfOrDie(fptr, "%lf", rot_ + i);
		}

		// Read in the translation estimate (from PnP)
		trans_ = new double[3];
		for(int i = 0; i < 3; ++i){
			fscanfOrDie(fptr, "%lf", trans_ + i);
		}



		// // Printing out data (for verification)
		// std::cout << "numViews: " << numViews_ << std::endl;
		// std::cout << "numPoints: " << numPts_ << std::endl;
		// std::cout << "numObs: " << numObs_ << std::endl;
		// std::cout << "K: " << K_[0] << " " << K_[1] << " " << K_[2] << " " << K_[3] << " " \
		// 	<< K_[4] << " " << K_[5] << " " << K_[6] << " " << K_[7] << " " << K_[8] << std::endl;
		// for(int i = 0; i < numObs_; ++i){
		// 	std::cout << "Obs: " << observations_[0+2*i] << " " << observations_[1+2*i] << std::endl;
		// }
		// for(int i = 0; i < numObs_; ++i){
		// 	std::cout << "ObsWeight: " << observationWeights_[i] << std::endl;
		// }
		// for(int i = 0; i < numObs_; ++i){
		// 	std::cout << "3D Point: " << X_bar_[0+3*i] << " " << X_bar_[1+3*i] << " " \
		// 	<< X_bar_[2+3*i] << std::endl;
		// }

		return true;

	}

private:

	// Helper function to read in one value to a text file
	template <typename T>
	void fscanfOrDie(FILE *fptr, const char *format, T *value){
		int numScanned = fscanf(fptr, format, value);
		if(numScanned != 1){
			LOG(FATAL) << "Invalid data file";
		}
	}

	// Private variables
	
	// Number of views
	int numViews_;
	// Number of keypoints
	int numPts_;
	// Number of observations
	int numObs_;

	// Center of the car
	double *carCenter_;
	// Dimensions of the car
	double h_, w_, l_;

	// Camera intrinsics
	double *K_;
	// Observation vector
	double *observations_;
	// Observation weight vector
	double *observationWeights_;
	// 3D point
	double *X_bar_;

	// Top 5 eigenvectors for the shape, i.e., the deformation basis vectors
	double *V_;

	// Weights for the eigenvectors
	double *lambdas_;

	// Rotation estimate (from PnP)
	double *rot_;
	// Translation estimate (from PnP)
	double *trans_;

};

// Read a problem file specified in 'my' format
class MultiViewShapeandPoseAdjuster{

public:

	// Return number of views.
	int getNumViews() const { return numViews_;}
	// Return the number of observations
	int getNumObs() const { return numObs_; }
	// Return the number of keypoints observed
	int getNumPts() const { return numPts_; }
	// Return the center of the car
	double* getCarCenter() { return carCenter_; }
	// Return the height of the car
	double getCarHeight() const { return h_; }
	// Return the width of the car
	double getCarWidth() const { return w_; }
	// Return the length of the car
	double getCarLength() const { return l_; }
	// Return a pointer to the observation vector
	double* observations() const { return observations_; }
	// Return a pointer to the observation weights vector
	double* observationWeights() const { return observationWeights_; }
	// Return a pointer to the camera intrinsics
	double* getK() const { return K_; }
	// Return a pointer to the mean 3D locations
	double* getX_bar() const { return X_bar_; }
	// Return a pointer to the top 5 eigenvectors
	double* getV() { return V_; }
	// Return a pointer to the weights (lambdas)
	double* getLambdas() { return lambdas_; }
	// Return the rotations
	double* getRotations() { return rot_; }
	// Return the translations.
	double* getTranslations() { return trans_; }

	// Read data from input file
	bool loadFile(const char *fileName){

		FILE *fptr = fopen(fileName, "r");
		if(fptr == NULL){
			return false;
		}

		// numViews, numPts, numObs, numFaces
		fscanfOrDie(fptr, "%d", &numViews_);
		fscanfOrDie(fptr, "%d", &numPts_);
		fscanfOrDie(fptr, "%d", &numObs_);

		// Height, Width, and Length of the Car
		fscanfOrDie(fptr, "%lf", &h_);
		fscanfOrDie(fptr, "%lf", &w_);
		fscanfOrDie(fptr, "%lf", &l_);


		// Camera intrinsics
		K_ = new double[9];
		for(int i = 0; i < 9; ++i){
			fscanfOrDie(fptr, "%lf", K_ + i);
		}

		// For each view,scan the following : center of the car, keypoints positions,keypoints weights,shape and vector.

		// Center of the car
		carCenter_ = new double[numViews_*3];
		for(int i=0;i<numViews_;++i){
			fscanfOrDie(fptr, "%lf", carCenter_ + 3*i +0);
			fscanfOrDie(fptr, "%lf", carCenter_ + 3*i +1);
			fscanfOrDie(fptr, "%lf", carCenter_ + 3*i +2);
		}

		/*for(int i=0;i<numViews_;i++){
			std::cout << carCenter_[3*i + 0] << " " << carCenter_[3*i + 1] << " " << carCenter_[3*i + 2] << std::endl;
		}*/
		
		// Keypoint Observations
		observations_ = new double[numViews_*2*numObs_];
		for(int i=0;i<numViews_;i++){
			for(int j = 0; j < numObs_; j++){
				for(int k = 0; k < 2; k++){
					fscanfOrDie(fptr, "%lf", observations_ + i*numObs_*2 + j*2 + k);
				}
			}
		}

		/*for(int i=0;i<numViews_;i++){
			for(int j = 0; j < numObs_; j++){
					std::cout << observations_[i*numObs_*2 + j*2 +0] << " " << observations_[i*numObs_*2 + j*2+ 1] << std::endl;		
			}
		}*/

		// Observation weights
		observationWeights_ = new double[numViews_*numObs_];
		for(int i=0;i<numViews_;i++){
			for(int j = 0; j < numObs_; ++j){
				fscanfOrDie(fptr, "%lf", observationWeights_ + i*numObs_ + j);
			}
		}
		/*for(int i=0;i<numViews_;i++){
			for(int j = 0; j < numObs_; j++){
					std::cout << observationWeights_[i*numObs_ + j]<< std::endl;		
			}
		}*/


	

		// Mean locations
		X_bar_ = new double[numViews_*3*numObs_];
		for(int i=0;i<numViews_;i++){
			for(int j = 0; j < numObs_; ++j){
				for(int k = 0; k < 3; ++k){
					fscanfOrDie(fptr, "%lf", X_bar_ + i*3*numObs_+ j*3 + k);
				}
			}
		}

		/*for(int i=0;i<numViews_;i++){
			for(int j = 0; j < numObs_; ++j){
				std::cout << X_bar_[i*3*numObs_+ j*3 + 0] << " " << X_bar_[i*3*numObs_+ j*3 + 1] << " " << X_bar_[i*3*numObs_+ j*3 + 2] <<std::endl;
			}
		}*/

		// Read in the top 5 eigenvectors for the shape
		// Size allocation: 5 vecs * 3 coordinates per vex * 14 keypoints (numPts_)
		V_ = new double[numViews_*42*3*numPts_];
		for(int i=0;i<numViews_;i++){
			for(int j = 0; j < 42; ++j){
				for(int k = 0; k < numPts_; ++k){
					fscanfOrDie(fptr, "%lf", V_ + i*3*42*numPts_ + j*3*numPts_ + 3*k + 0);
					fscanfOrDie(fptr, "%lf", V_ + i*3*42*numPts_ + j*3*numPts_ + 3*k + 1);
					fscanfOrDie(fptr, "%lf", V_ + i*3*42*numPts_ + j*3*numPts_ + 3*k + 2);
			
				}
			}
		}
		

		/*for(int i=0;i<1;i++){
			for(int j = 0; j < 5; ++j){
				for(int k = 0; k < numPts_; ++k){
					std::cout << V_[i*3*5*numPts_ + j*3*numPts_ + 3*k + 0] << " " << V_[i*3*5*numPts_ + j*3*numPts_ + 3*k + 1] << " " << V_[i*3*5*numPts_ + j*3*numPts_ + 3*k + 2] <<std::endl;
				}
			}
		}*/


		// Read in the initial values for lambdas
		lambdas_ = new double[42];
		for(int i=0;i<42;i++){
			fscanfOrDie(fptr, "%lf", lambdas_ + i);
		}




		// Read in the rotation estimate (from PnP) (column-major ordered rotation matrix)
		rot_ = new double[numViews_*9];
		for(int i = 0; i < numViews_; i++){
			for(int j=0;j<9;j++){
				fscanfOrDie(fptr, "%lf", rot_ + i*9 + j);
			}
		}
		
		// Read in the translation estimate (from PnP)
		trans_ = new double[numViews_*3];
		for(int i = 0; i < numViews_; i++){
			for(int j=0;j<3;j++){
				fscanfOrDie(fptr, "%lf", trans_ + i*3 + j);
			}
		}

		//std::cout << trans_[0] << " "  << trans_[1] <<" "  << trans_[2]<<std::endl;




		return true;


	}

private:

	// Helper function to read in one value to a text file
	template <typename T>
	void fscanfOrDie(FILE *fptr, const char *format, T *value){
		int numScanned = fscanf(fptr, format, value);
		if(numScanned != 1){
			LOG(FATAL) << "Invalid data file";
		}
	}

	// Private variables
	
	// Number of views
	int numViews_;
	// Number of keypoints
	int numPts_;
	// Number of observations
	int numObs_;

	// Center of the car
	double *carCenter_;
	// Dimensions of the car
	double h_, w_, l_;

	// Camera intrinsics
	double *K_;
	// Observation vector
	double *observations_;
	// Observation weight vector
	double *observationWeights_;
	// 3D point
	double *X_bar_;

	// Top 5 eigenvectors for the shape, i.e., the deformation basis vectors
	double *V_;
	// Weights for the eigenvectors
	double *lambdas_;
	// Rotations in each frame.
	double *rot_;

	// Translation for each frame.
	double *trans_;

};
