#include <MpsQ.h>
#include <MpoQ.h>
#include <MpKondoModel.h>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

/**Calculating observables for given sets of lattice sites. 
\describe_Nq
\describe_Scalar
\describe_Hamiltonian*/
template <size_t Nq, typename Scalar, typename Hamiltonian> class Observables
{
public:

	Observables<Nq,Scalar,Hamiltonian> (const MpsQ<Nq,Scalar> * stateInput, const Hamiltonian * Hinput);
	
	Eigen::MatrixXd ImpSubCorr (vector<size_t> impLocs, vector<size_t> subLocs);
	Eigen::MatrixXd ImpCorr (vector<size_t> imp1Locs, vector<size_t> imp2Locs);
	Eigen::MatrixXd SizVal (vector<size_t> locs);
	Eigen::MatrixXd localPksCorr (vector<size_t> locs1, vector<size_t> locs2);
	Eigen::MatrixXd localPksCorrMF (vector<size_t> locs1, vector<size_t> locs2, Eigen::MatrixXd * ImpSubCorr);
	Eigen::Tensor<double,3> PksCorr ();
	Eigen::Tensor<double,3> PksCorrMF (Eigen::MatrixXd * ImpSubCorr, Eigen::MatrixXd * ImpCorr);

private:
	string info;
	MpsQ<Nq,Scalar> state;
	Hamiltonian H;
};

template <size_t Nq, typename Scalar, typename Hamiltonian> Observables<Nq,Scalar,Hamiltonian> ::
Observables (const MpsQ<Nq,Scalar> * stateInput, const Hamiltonian * Hinput)
{
	state = *stateInput;
	H = *Hinput;
}

template <size_t Nq, typename Scalar, typename Hamiltonian>
Eigen::MatrixXd Observables<Nq,Scalar,Hamiltonian>::
ImpSubCorr (vector<size_t> impLocs, vector<size_t> subLocs)
{
	Eigen::MatrixXd ImpSubCorr(impLocs.size(),subLocs.size());
	ImpSubCorr.setZero();
	
	for (size_t i=0; i<impLocs.size(); ++i)
	{
		for (size_t j=0; j<subLocs.size(); ++j)
		{
			ImpSubCorr(i,j) = avg(this->state, (this->H).SimpSsub((this->H).length(),impLocs[i],SZ,subLocs[j],SZ), this->state) +
				0.5*(avg(this->state , (this->H).SimpSsub((this->H).length(),impLocs[i],SP,subLocs[j],SM), this->state)
					 + avg(this->state , (this->H).SimpSsub((this->H).length(),impLocs[i],SM,subLocs[j],SP), this->state));
		}
	}
	return ImpSubCorr;
}

template <size_t Nq, typename Scalar, typename Hamiltonian>
Eigen::MatrixXd Observables<Nq,Scalar,Hamiltonian>::
ImpCorr (vector<size_t> imp1Locs, vector<size_t> imp2Locs)
{
	Eigen::MatrixXd ImpCorr(imp1Locs.size(),imp2Locs.size());
	ImpCorr.setZero();

	for (size_t i=0; i<imp1Locs.size(); ++i)
	{
		for (size_t j=0; j<imp2Locs.size(); ++j)
		{
			ImpCorr(i,j) = avg(this->state, (this->H).SimpSimp((this->H).length(),imp1Locs[i],SZ,imp2Locs[j],SZ), this->state) +
				0.5*(avg(this->state , (this->H).SimpSimp((this->H).length(),imp1Locs[i],SP,imp2Locs[j],SM), this->state)
					 + avg(this->state , (this->H).SimpSimp((this->H).length(),imp1Locs[i],SM,imp2Locs[j],SP), this->state));
		}
	}
	return ImpCorr;
}

template <size_t Nq, typename Scalar, typename Hamiltonian>
Eigen::MatrixXd Observables<Nq,Scalar,Hamiltonian>::
SizVal (vector<size_t> locs)
{
	Eigen::MatrixXd SizVal(locs.size(),1);
	SizVal.setZero();
	for (size_t i=0; i<locs.size(); i++)
	{
		SizVal(i) = avg(this->state , (this->H).Simp((this->H).length(),locs[i],SZ) , this->state);
	}

	return SizVal;
}

template <size_t Nq, typename Scalar, typename Hamiltonian>
Eigen::MatrixXd Observables<Nq,Scalar,Hamiltonian>::
localPksCorr (vector<size_t> locs1, vector<size_t> locs2)
{
	Eigen::MatrixXd localPksCorr(locs1.size(),locs2.size());
	localPksCorr.setZero();
	
	for (size_t i=0; i<locs1.size(); i++)
	{
		for (size_t j=i+1; j<locs2.size(); j++)
		{
			localPksCorr(i,j) = avg(this->state, (this->H).SimpSsubSimpSsub(this->H.length(),locs1[i],SZ,locs1[i],SZ,locs2[j],SZ,locs2[j],SZ), this->state)
				+ 0.5*avg(this->state, (this->H).SimpSsubSimpSsub(this->H.length(),locs1[i],SZ,locs1[i],SZ,locs2[j],SP,locs2[j],SM), this->state)
				+ 0.5*avg(this->state, (this->H).SimpSsubSimpSsub(this->H.length(),locs1[i],SZ,locs1[i],SZ,locs2[j],SM,locs2[j],SP), this->state)
				+ 0.5*avg(this->state, (this->H).SimpSsubSimpSsub(this->H.length(),locs1[i],SP,locs1[i],SM,locs2[j],SZ,locs2[j],SZ), this->state)
				+ 0.25*avg(this->state, (this->H).SimpSsubSimpSsub(this->H.length(),locs1[i],SP,locs1[i],SM,locs2[j],SP,locs2[j],SM), this->state)
				+ 0.25*avg(this->state, (this->H).SimpSsubSimpSsub(this->H.length(),locs1[i],SP,locs1[i],SM,locs2[j],SM,locs2[j],SP), this->state)
				+ 0.5*avg(this->state, (this->H).SimpSsubSimpSsub(this->H.length(),locs1[i],SM,locs1[i],SP,locs2[j],SZ,locs2[j],SZ), this->state)
				+ 0.25*avg(this->state, (this->H).SimpSsubSimpSsub(this->H.length(),locs1[i],SM,locs1[i],SP,locs2[j],SP,locs2[j],SM), this->state)
				+ 0.25*avg(this->state, (this->H).SimpSsubSimpSsub(this->H.length(),locs1[i],SM,locs1[i],SP,locs2[j],SM,locs2[j],SP), this->state);
		}
	}
	return localPksCorr;
}

template <size_t Nq, typename Scalar, typename Hamiltonian>
Eigen::MatrixXd Observables<Nq,Scalar,Hamiltonian>::
localPksCorrMF (vector<size_t> locs1, vector<size_t> locs2, Eigen::MatrixXd * ImpSubCorr)
{
	Eigen::MatrixXd localPksCorrMF(locs1.size(),locs2.size());
	localPksCorrMF.setZero();
	
	for (size_t i=0; i<locs1.size(); i++)
	{
		for (size_t j=i+1; j<locs2.size(); j++)
		{
			localPksCorrMF(i,j) = (*ImpSubCorr)(i,i) * (*ImpSubCorr)(j,j);
		}
	}
	return localPksCorrMF;	
}

template <size_t Nq, typename Scalar, typename Hamiltonian>
Eigen::Tensor<double,3> Observables<Nq,Scalar,Hamiltonian>::
PksCorr ()
{
	size_t L = (this->H).length();
	Eigen::Tensor<double,3> PksCorr(L,L,L);
	PksCorr.setZero();

	size_t triangle[] = {0,1,2};
	for (size_t i=0; i<(H.length() -2); i++)
	{
		size_t j=i+1;
		size_t k=j+1;
		triangle[0] = i;
		triangle[1] = j;
		triangle[2] = k;
		do {
			PksCorr(triangle[0],triangle[1],triangle[2]) = avg(this->state, (this->H).SimpSsubSimpSimp(L,triangle[0],SZ,triangle[0],SZ,triangle[1],SZ,triangle[2],SZ), this->state)
			+ 0.5*avg(this->state, (this->H).SimpSsubSimpSimp(L,triangle[0],SZ,triangle[0],SZ,triangle[1],SP,triangle[2],SM), this->state)
			+ 0.5*avg(this->state, (this->H).SimpSsubSimpSimp(L,triangle[0],SZ,triangle[0],SZ,triangle[1],SM,triangle[2],SP), this->state)
			+ 0.5*avg(this->state, (this->H).SimpSsubSimpSimp(L,triangle[0],SP,triangle[0],SM,triangle[1],SZ,triangle[2],SZ), this->state)
			+ 0.25*avg(this->state, (this->H).SimpSsubSimpSimp(L,triangle[0],SP,triangle[0],SM,triangle[1],SP,triangle[2],SM), this->state)
			+ 0.25*avg(this->state, (this->H).SimpSsubSimpSimp(L,triangle[0],SP,triangle[0],SM,triangle[1],SM,triangle[2],SP), this->state)
			+ 0.5*avg(this->state, (this->H).SimpSsubSimpSimp(L,triangle[0],SM,triangle[0],SP,triangle[1],SZ,triangle[2],SZ), this->state)
			+ 0.25*avg(this->state, (this->H).SimpSsubSimpSimp(L,triangle[0],SM,triangle[0],SP,triangle[1],SP,triangle[2],SM), this->state)
			+ 0.25*avg(this->state, (this->H).SimpSsubSimpSimp(L,triangle[0],SM,triangle[0],SP,triangle[1],SM,triangle[2],SP), this->state);
		} while ( std::next_permutation(triangle,triangle+3) );
	}
	return PksCorr;
}

template <size_t Nq, typename Scalar, typename Hamiltonian>
Eigen::Tensor<double,3> Observables<Nq,Scalar,Hamiltonian>::
PksCorrMF (Eigen::MatrixXd * ImpSubCorr, Eigen::MatrixXd * ImpCorr)
{
	size_t L = (this->H).length();	
	Eigen::Tensor<double,3> PksCorrMF(L,L,L);
	PksCorrMF.setZero();
	
	size_t triangle[] = {0,1,2};
	for (size_t i=0; i<((this->H).length() -2); i++)
	{
		size_t j=i+1;
		size_t k=j+1;
		triangle[0] = i;
		triangle[1] = j;
		triangle[2] = k;
		do {
			PksCorrMF(triangle[0],triangle[1],triangle[2]) = (*ImpSubCorr)(triangle[0],triangle[0])*(*ImpCorr)(triangle[1],triangle[2]);
		} while ( std::next_permutation(triangle,triangle+3) );
	}
	return PksCorrMF;
}

