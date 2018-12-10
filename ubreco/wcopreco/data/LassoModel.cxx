#include "LassoModel.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>
using namespace Eigen;

#include <iostream>
using namespace std;


/* Minimize the following problem:
 * 1/(2) * ||Y - beta * X||_2^2 + N * lambda * ||beta||_1
 */

wcopreco::LassoModel::LassoModel(double lambda, int max_iter, double TOL, bool non_negtive)
: ElasticNetModel(lambda, 1., max_iter, TOL, non_negtive)
, flag_initial_values(false)
{
    name = "Lasso";
}

wcopreco::LassoModel::~LassoModel()
{}

void wcopreco::LassoModel::Set_init_values(std::vector<double> values){
  flag_initial_values = true;
  init_betas = values;
}

void wcopreco::LassoModel::Fit()
{
  // initialize solution to zero
  Eigen::VectorXd beta = VectorXd::Zero(_X.cols());

  if (flag_initial_values){
    for (int i=0;i!=beta.size();i++){
      beta(i) = init_betas.at(i);
    }
  }

  // initialize active_beta to true
  int nbeta = beta.size();
  _active_beta = vector<bool>(nbeta, true);

  Eigen::VectorXd y = Gety();
  Eigen::MatrixXd X = GetX();

  // cooridate decsent
  //int N = y.size();
  VectorXd norm(nbeta);
  for (int j=0; j<nbeta; j++) {
    norm(j) = X.col(j).squaredNorm();
    if (norm(j) < 1e-6) {
      cerr << "warning: the " << j << "th variable is not used, please consider removing it." << endl;
      norm(j) = 1;
    }
  }
  double tol2 = TOL*TOL*nbeta;

  // calculate the inner product
  Eigen::VectorXd ydX(nbeta);
  Eigen::SparseMatrix<double> XdX(nbeta,nbeta);
  for (int i=0;i!=nbeta;i++){
    ydX(i) = y.dot(X.col(i));
    //beta(i) = ydX(i) / norm(i); // first time result saved here ...
    for (int j=0;j!=nbeta;j++){
      double value = X.col(i).dot(X.col(j));
      if (value!=0)
	XdX.insert(i,j) = value;
      //std::cout << i << " " << j << " " << value << " " << XdX.coeffRef(i,j) << std::endl;
    }
    //std::cout << ydX(i) << " " << norm(i) << std::endl;
  }
  //  std::cout << "Xin " << nbeta << std::endl;


  // start interation ...
  int double_check  = 0;
  for (int i =0; i< max_iter; i++){
    VectorXd betalast = beta;

    //loop through sparse matrix ...
    for (int j=0;j!=nbeta;j++){
      if (!_active_beta[j]) {continue;}
      beta(j) = ydX(j);
      for (SparseMatrix<double>::InnerIterator it(XdX,j); it; ++it){
	//if (it.row()!=j && beta(it.row())!=0)
	if (it.row()!=j)
	  beta(j) -= it.value() * beta(it.row());
      }
      beta(j) = _soft_thresholding( beta(j)/norm(j), lambda * lambda_weight(j));

      if(fabs(beta(j)) < 1e-6) { _active_beta[j] = false; }
     // VectorXd X_j = X.col(j);
     // VectorXd beta_tmp = betalast;
     // beta_tmp(j) = 0;
     // VectorXd r_j = (y - X * beta_tmp);
     // double delta_j = X_j.dot(r_j);
      //std::cout << i << " " << j << " " << beta(j) << " " << betalast(j) << std::endl;

    }
    double_check++;
    // cout << endl;
    VectorXd diff = beta - betalast;

    // std::cout << i << " " << diff.squaredNorm() << " " << tol2 << std::endl;

    if (diff.squaredNorm()<tol2) {
      if (double_check!=1) {
      	double_check = 0;
      	for (int k=0; k<nbeta; k++) {
      	  _active_beta[k] = true;
      	}
      }else {
	//cout << "found minimum at iteration: " << i << " " << flag_initial_values << endl;
	break;
      }
    }
  }


  // save results in the model
  Setbeta(beta);
}

double wcopreco::LassoModel::chi2_l1()
{
  return 2 * lambda * Getbeta().lpNorm<1>() * Gety().size() ;
}
