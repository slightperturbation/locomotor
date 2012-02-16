/// $Id: Math.hpp,v 1.1.2.1 2006/04/20 08:13:36 dexabraxis Exp $
/// $URL: http://www.luminousbeings.com/svn/laetoli/trunk/Laetoli/Math/Math.hpp $
/// \author Brian "Dexter" Allen (vector@acm.org)
/// Copyright 2009. 
#ifndef LAETOLI_MATH_HPP
#define LAETOLI_MATH_HPP

#include <cmath>
#include <algorithm>
#include <ctime> // for std::time
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/random.hpp>
#include <gmtl/gmtl.h>
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define PI_OVER_2 (1.57079633)

namespace laetoli
{

	//////////////////////////////////////////////////////////////////////////
	/// Vector is a one-dimensional array of size tSize and type tScalar.
	template< typename tScalar, size_t tSize > class Vector
	{
	public:
		enum { ourSize = tSize };
		typedef tScalar ourType;

		//////////////////////////////////////////////////////////////////////////
		/// forwardIterator an STL-compatible forward iterator.  
		/// Uses value-semantics.
		class forwardIterator 
			: public std::iterator< std::forward_iterator_tag, Vector< tScalar, tSize > >
		{
		public:


			forwardIterator( Vector< tScalar, tSize >& argVector ) 
				: myIndex( 0 ), 
					myVector( argVector ) 
			{ }
			forwardIterator( const forwardIterator& argOther ) 
				: myIndex( argOther.myIndex ),
					myVector( argOther.myVector )
			{ }
			forwardIterator( Vector< tScalar, tSize >& argVector, int argIndex )
				: myIndex( argIndex ),
					myVector( argVector )
			{ }
			forwardIterator& operator=( const forwardIterator& argOther ) 
			{
				myVector = argOther.myVector;
				myIndex = argOther.myIndex;
				return *this;
			}
			tScalar& operator*( void )
			{
				return myVector.get( myIndex );
			}
			const tScalar& operator*( void ) const
			{
				return myVector.get( myIndex );
			}
			void operator++() // prefix
			{
				++myIndex;
			}
			void operator++( int ) // postfix 
			{
				++myIndex;
			}
			bool operator==( const forwardIterator& argOther ) const
			{
				return ((&myVector) == (&argOther.myVector)) && (myIndex == argOther.myIndex);
			}
			bool operator!=( const forwardIterator& argOther ) const
			{
				return ((&myVector) != (&argOther.myVector)) || (myIndex != argOther.myIndex);
			}

            void normalize( void )
            {
                tScalar len = length();
                for( int i = 0; i < tSize; ++i )
                {
                    (*this)[i] /= len;
                }
            }

		private:
			int myIndex;
			Vector< tScalar, tSize >& myVector;
		};
		/// Iterator typedef for STL-compatibility.
		typedef forwardIterator iterator;

		class const_forwardIterator : public forwardIterator
		{
		public:
			const_forwardIterator( const Vector< tScalar, tSize >& argVector ) 
				: forwardIterator( 0, argVector ) 
			{ }
			const_forwardIterator( const const_forwardIterator& argSource )
				: forwardIterator( argSource )
			{ }
			const_forwardIterator( const forwardIterator& argSource )
				: forwardIterator( argSource )
			{ }
			const_forwardIterator( const Vector< tScalar, tSize >& argVector, int argIndex )
				: forwardIterator( const_cast< Vector< tScalar, tSize >& >(argVector), argIndex )
			{ }
			const_forwardIterator( Vector< tScalar, tSize >& argVector, int argIndex )
				: forwardIterator( argVector, argIndex )
			{ }
			const_forwardIterator& operator=( const const_forwardIterator& argOther ) 
			{
				return forwardIterator::operator=( argOther );
			}
			bool operator==( const const_forwardIterator& argOther ) const
			{
				return forwardIterator::operator==( argOther );
			}
		};
		typedef const_forwardIterator const_iterator;

		/// Returns an iterator pointing to the first element in 
		/// the Vector.
		iterator begin( void )
		{
			return iterator( *this, 0 );
		}

		const_iterator begin( void ) const
		{
			return const_iterator( *this, 0 );
		}

		/// Returns an iterator pointing one place past the last 
		/// element in the Vector.
		iterator end( void )
		{
			return iterator( *this, tSize );
		}

		const_iterator end( void ) const
		{
			return const_iterator( *this, tSize );
		}

		//////////////////////////////////////////////////////////////////////////
		// Accessors and Mutators

		/// Returns a constant reference to the element at the argument's 
		/// position.
		const tScalar& get( int argIndex ) const
		{ 
			if( argIndex < 0 || argIndex > tSize - 1 ) 
			{
				throw "Vector index out of bounds.";
			}
			return myData[ argIndex ]; 
		}

		/// Returns a mutable reference to the element at the argument's 
		/// position.
		tScalar& get( int argIndex )
		{ 
			if( argIndex < 0 || argIndex > tSize - 1 ) 
			{
				throw "Vector index out of bounds.";
			}
			return myData[ argIndex ]; 
		}

		/// Returns a constant reference to the element at the argument's 
		/// position.
		const tScalar& operator[]( int argIndex ) const
		{
			return get( argIndex );
		}

		/// Returns a mutable reference to the element at the argument's 
		/// position.
		tScalar& operator[]( int argIndex )
		{
			if( argIndex < 0 || argIndex > tSize - 1 ) 
			{
				throw "Vector index out of bounds.";
			}
			return myData[argIndex];
		}

		//////////////////////////////////////////////////////////////////////////
		// Operators

		/// Unary Minus.  Returns a vector that with each component multiplied by -1.
		Vector< tScalar, tSize > operator-( void ) const
		{
			Vector< tScalar, tSize > retVec( *this );
			for( int i = 0; i < tSize; ++i )
			{
				retVec[i] *= -1;
			}
			return retVec;
		}	

		/// Scalar multiplication.  Multiplies every element in the vector
		/// by the provided tScalar factor.
		Vector< tScalar, tSize > operator*( tScalar argFactor ) const
		{
			Vector< tScalar, tSize > retVec( *this );
			for( int i = 0; i < tSize; ++i )
			{
				retVec[i] *= argFactor;
			}
			return retVec;
		}

		/// Scalar division.  Divides every element in the vector
		/// by the provided tScalar factor.
		Vector< tScalar, tSize > operator/( tScalar argFactor ) const
		{
			Vector< tScalar, tSize > retVec( *this );
			for( int i = 0; i < tSize; ++i )
			{
				retVec[i] /= argFactor;
			}
			return retVec;
		}

		/// Scalar multiplication.  Returns a new vector with every element 
		/// in the vector multiplied by the provided tScalar factor.
		Vector< tScalar, tSize >& operator*=( tScalar argFactor )
		{
			for( int i = 0; i < tSize; ++i )
			{
				myData[i] *= argFactor;
			}
			return *this;
		}

		/// Scalar divides.  Returns a new vector with every element 
		/// in the vector divided by the provided tScalar factor.
		Vector< tScalar, tSize >& operator/=( tScalar argFactor )
		{
			for( int i = 0; i < tSize; ++i )
			{
				myData[i] /= argFactor;
			}
			return *this;
		}

		//////////////////////////////////////////////////////////////////////////
		// Member functions

		/// Returns a unit-length vector indicating the direction of this vector.
		Vector< tScalar, tSize> direction( void ) const
		{
			tScalar len = length();
			if( len == static_cast< tScalar >( 0 ) ) 
			{
				return Vector< tScalar, tSize >( static_cast< tScalar >( 0 ) );
			}
			Vector< tScalar, tSize > norm( *this );
			norm /= len;
			return norm;
		}

		/// Returns the (always positive) length of the vector. 
		tScalar length( void ) const
		{
			tScalar len = 0;
			for( int i = 0; i < tSize; ++i )
			{
				len += myData[i]*myData[i];
			}
			len = std::sqrt( len );
			return len;
		}

		//////////////////////////////////////////////////////////////////////////
		// Free friend functions	

		/// Scalar multiplication.  Multiplies every element in the vector
		/// by the provided tScalar factor.
		friend 
			Vector< tScalar, tSize > operator* ( tScalar argFactor, 
			const Vector< tScalar, tSize >& argVec )
		{
			Vector< tScalar, tSize > retVec( argVec );
			for( int i = 0; i < tSize; ++i )
			{
				retVec[i] *= argFactor;
			}
			return retVec;
		}

		/// Scalar division.  Divides every element in the vector
		/// by the provided tScalar factor.
		friend 
			Vector< tScalar, tSize > operator/ ( tScalar argFactor, 
			const Vector< tScalar, tSize >& argVec )
		{
			Vector< tScalar, tSize > retVec( argVec );
			for( int i = 0; i < tSize; ++i )
			{
				retVec[i] /= argFactor;
			}
			return retVec;
		}

		/// Returns true if every element of equal-sized vectors is equal.
		friend bool operator== ( const Vector< tScalar, tSize >& argLHS, 
			const Vector< tScalar, tSize >& argRHS )
		{
			for( int i = 0; i < tSize; ++i )
			{
				if( argLHS[i] != argRHS[i] )
				{
					return false;
				}
			}
			return true;
		}

		/// Returns true if any position differs between the two Vectors.
		friend bool operator!= ( const Vector< tScalar, tSize >& argLHS, 
			const Vector< tScalar, tSize >& argRHS )
		{
			return !( argLHS == argRHS );
		}

		/// Normalizes the vector argument.
        friend void normalizeVec( Vector< tScalar, tSize >& argToBeNormalized )
		{
			tScalar len = argToBeNormalized.length();
			for( int i = 0; i < tSize; ++i )
			{
				argToBeNormalized[i] /= len;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Constructors 
		/// Default constructor.  All elements initialized to zero.
		Vector( void )
		{
			for( int i = 0; i < tSize; ++i )
			{
				myData[i] = 0;
			}
		}

		explicit Vector( tScalar argDefaultValue )
		{
			for( int i = 0; i < tSize; ++i )
			{
				myData[i] = argDefaultValue;
			}
		}

		explicit Vector( const tScalar* argPODArray )
		{
			for( int i = 0; i < tSize; ++i )
			{
				myData[i] = argPODArray[i];
			}
		}

		//TODO is there a nicer partial spec way to do this....?
		explicit Vector( tScalar argA, tScalar argB, tScalar argC )
		{
			myData[0] = argA; myData[1] = argB; myData[2] = argC;
		}

		//TODO is there a nicer partial spec way to do this....?
		explicit Vector ( tScalar argA, tScalar argB, tScalar argC, tScalar argD )
		{
			myData[0] = argA; myData[1] = argB; myData[2] = argC; myData[3] = argD;
		}


		/// Conversion to POD Array
		const tScalar* toArray( void ) const
		{
			return &(myData[0]);
		}

		/// Conversion to POD Array
		tScalar* toArray( void )
		{
			return &(myData[0]);
		}


		/// Copy constructor.  Can copy from a Vector of a different scalar
		/// type as long as the source scalar can be static_cast'd to the 
		/// target vector's scalar type.
		template< typename tScalar2 >
		Vector( const Vector< tScalar2, tSize >& argSource )
		{
			for( int i = 0; i < tSize; ++i )
			{
				myData[i] = static_cast< tScalar >( argSource[i] );
			}
		}

	private:
		tScalar myData[tSize];
	};


	// Convenience Types
	typedef Vector< float, 3 > Point3f;
	typedef Vector< double, 3 > Point3d;
	typedef Vector< float, 3 > Vec3f;
	typedef Vector< double, 3 > Vec3d;
	typedef Vector< float, 4 > Vec4f;
	typedef Vector< double, 4 > Vec4d;
	typedef Vector< double, 16 > Matrix44d;
	typedef Vec3d Color;
	
	
	/// dot-product
	/// the sum of the products of respective elements
	template< typename tScalar, size_t tSize >
	tScalar dot( const Vector< tScalar, tSize >& argFirst, const Vector< tScalar, tSize >& argSecond )
	{
		tScalar retVal = 0;
		for( int i = 0; i < tSize; ++i )
		{
			retVal += argFirst[i] * argSecond[i];
		}
		return retVal;
	}

	template< typename tScalar, size_t tSize >
	double angleTo( const Vector< tScalar, tSize >& argFrom, const Vector< tScalar, tSize >& argTo )
	{
        laetoli::Vec3d from = argFrom;
        tScalar lenFrom = from.length();
        for( int i = 0; i < tSize; ++i )
        {
            from[i] /= lenFrom;
        }
        laetoli::Vec3d to = argTo;
        tScalar lenTo = to.length();
        for( int i = 0; i < tSize; ++i )
        {
            to[i] /= lenTo;
        }
        double d = dot( from, to );
		assert( d >= -1 && d <= 1 );
		return acos( d );
	}

	/// Cross-product for 3-dimension vectors
	template< typename tScalar >
	Vector< tScalar, 3 >& cross( Vector< tScalar, 3 >& outResult, 
								const Vector< tScalar, 3 >& argLHS, 
								const Vector< tScalar, 3 >& argRHS )
	{
		const Vector< tScalar, 3 >&a = argLHS;  
		const Vector< tScalar, 3 >&b = argRHS;  
		outResult[0] = a[1]*b[2] - a[2]*b[1];
		outResult[1] = a[2]*b[0] - a[0]*b[2]; // note flipping for negative cofactor
		outResult[2] = a[0]*b[1] - a[1]*b[0];
		return outResult;
	}

	/// 3Vec-Matrix product
	template< typename tScalar >
	Vector< tScalar, 3 > multiplyVector( Vector< tScalar, 3 > argVector,
										const Vector< tScalar, 16 >& argM )
	{
		Vector< tScalar, 4 > vec;
		for( int i = 0; i < 3; ++i ) vec[i] = argVector[i];
		vec[3] = 0.0;
		vec = multiply( vec, argM );
		Vector< tScalar, 3 > out;
		for( int i = 0; i < 3; ++i ) out[i] = vec[i];
		return out;
	}
	/// Matrix-3Vec product
	template< typename tScalar >
	Vector< tScalar, 3 > multiplyVector( const Vector< tScalar, 16 >& argM,
										Vector< tScalar, 3 > argVector )
	{
		Vector< tScalar, 4 > vec;
		for( int i = 0; i < 3; ++i ) vec[i] = argVector[i];
		vec[3] = 0.0;
		vec = multiply( argM, vec );
		Vector< tScalar, 3 > out;
		for( int i = 0; i < 3; ++i ) out[i] = vec[i];
		return out;
	}

	/// 3Point-Matrix product
	template< typename tScalar >
	Vector< tScalar, 3 > multiplyPoint( Vector< tScalar, 3 > argPoint,
									   const Vector< tScalar, 16 >& argM )
	{
		Vector< tScalar, 4 > pt;
		for( int i = 0; i < 3; ++i ) pt[i] = argPoint[i];
		pt[3] = 1.0;
		pt = multiply( pt, argM );
		Vector< tScalar, 3 > out;
		for( int i = 0; i < 3; ++i ) out[i] = pt[i];
		return out;
	}
	/// Matrix-3Point product
	template< typename tScalar >
	Vector< tScalar, 3 > multiplyPoint( const Vector< tScalar, 16 >& argM, 
									   Vector< tScalar, 3 > argPoint )
	{
		Vector< tScalar, 4 > pt;
		for( int i = 0; i < 3; ++i ) pt[i] = argPoint[i];
		pt[3] = 1.0;
		pt = multiply( argM, pt );
		Vector< tScalar, 3 > out;
		for( int i = 0; i < 3; ++i ) out[i] = pt[i];
		return out;
	}


	/// Vector-Matrix product
	/// TODO Very inefficient!
	template< typename tScalar >
	Vector< tScalar, 4 > multiply( Vector< tScalar, 4 > argVector,
								  const Vector< tScalar, 16 >& argM )
	{
		Vector< tScalar, 4 > out;
		Vector< tScalar, 4 > col;
		for( int i = 0 ; i < 4; ++i )
		{
			for( int j = 0 ; j < 4; ++j )
			{
				col[j] = argM[ (j * 4) + i ];
			}
			out[i] = dot( argVector, col );
		}
		return out;
	}

	/// Matrix-Vector product
	/// TODO Very inefficient!
	template< typename tScalar >
	Vector< tScalar, 4 > multiply( const Vector< tScalar, 16 >& argM,
								  Vector< tScalar, 4 > argVector )
	{
		Vector< tScalar, 4 > out;
		Vector< tScalar, 4 > row;
		for( int i = 0 ; i < 4; ++i )
		{
			for( int j = 0 ; j < 4; ++j )
			{
				row[j] = argM[ j + (i*4) ];
			}
			out[i] = dot( argVector, row );
		}
		return out;
	}
/*
	/// Inverse of 4x4 Matrix
	/// TODO fix massive copying
	template< typename tScalar >
	Vector< tScalar, 16 > invert( const Vector< tScalar, 16 >& argM )
	{
		// manky annoyance to use the provided invert op.
		double tmp[4][4];
		for( int i = 0; i < 4; ++i )
		{
			for( int j = 0; j < 4; j++ )
			{
				tmp[i][j] = argM[ i + (j*4) ];
			}
		}
		double out[4][4];

		invert_matrix( tmp, out );

		Vector< tScalar, 16 > retVal;
		for( int i = 0; i < 4; ++i )
		{
			for( int j = 0; j < 4; j++ )
			{
				retVal[ i + (j*4) ] = out[i][j];
			}
		}
		return retVal;
	}
*/
	/// Sum op
	template< typename tScalar, size_t tSize >
	Vector< tScalar, tSize > operator+( const Vector< tScalar, tSize >& argLHS, 
									   const Vector< tScalar, tSize >& argRHS )
	{
		Vector< tScalar, tSize > out;
		for( int i = 0; i < tSize; ++i )
		{
			out[i] = argLHS[i] + argRHS[i];
		}
		return out;
	}

	/// Diff op
	template< typename tScalar, size_t tSize >
	Vector< tScalar, tSize > operator-( const Vector< tScalar, tSize >& argLHS, 
									   const Vector< tScalar, tSize >& argRHS )
	{
		Vector< tScalar, tSize > out;
		for( int i = 0; i < tSize; ++i )
		{
			out[i] = argLHS[i] - argRHS[i];
		}
		return out;
	}

	/// Mult op
	template< typename tScalar, size_t tSize >
	Vector< tScalar, tSize > operator*( tScalar argScalar, 
									   const Vector< tScalar, tSize >& argRHS )
	{
		Vector< tScalar, tSize > out;
		for( int i = 0; i < tSize; ++i )
		{
			out[i] = argScalar * argRHS[i];
		}
		return out;
	}

	/// Mult op
	template< typename tScalar, size_t tSize >
	Vector< tScalar, tSize > operator*( const Vector< tScalar, tSize >& argRHS, 
									   tScalar argScalar )
	{
		Vector< tScalar, tSize > out;
		for( int i = 0; i < tSize; ++i )
		{
			out[i] = argScalar * argRHS[i];
		}
		return out;
	}

	/// Component-wise Mult 
	template< typename tScalar, size_t tSize >
	Vector< tScalar, tSize > operator*( const Vector< tScalar, tSize >& argRHS, 
									   const Vector< tScalar, tSize >& argLHS )
	{
		Vector< tScalar, tSize > out;
		for( int i = 0; i < tSize; ++i )
		{
			out[i] = argRHS[i] * argLHS[i];
		}
		return out;
	}

	/// projection, returns a vector that's the projection of the first onto the second
	template< typename tScalar, size_t tSize >
	Vector< tScalar, tSize > proj( const Vector< tScalar, tSize >& argFirst, const Vector< tScalar, tSize >& argOnto )
	{
		Vector< tScalar, tSize > out;
		assert( argOnto.length() != 0 );
		out = ( dot( argFirst, argOnto ) / dot( argOnto, argOnto ) ) * argOnto;
		return out;
	}



	struct Ray
	{
		Point3d myOrigin;
		Vec3d myDirection;
	};


	typedef Vector< float, 16 > MatrixColMajor4x4f;
	typedef Vector< double, 16 > MatrixColMajor4x4d;

	//////////////////////////////////////////////////////////////////////////
	// Random numbers

	class Entropy
	{
		// Random number types
		typedef boost::mt19937 base_generator_type;

	public:
		explicit Entropy( void )
#ifdef _DEBUG
			: myRandGen( 0 ),
#else
			: myRandGen( static_cast<unsigned int>( std::time(0) ) ),
#endif
			myNormalDist( 0, 1 ),
			myUniformDist( 0, 1 ),
			myNormalGenerator( myRandGen, myNormalDist ),  // note! dependent on declare order below
			myUniformGenerator( myRandGen, myUniformDist )  // note! dependent on declare order below
		{ }
		double gauss( void )
		{
			return myNormalGenerator();
		}

		double gauss( double argMean, double argSigma )
		{
			using namespace boost;
			normal_distribution<double> normDist( argMean, argSigma );
			variate_generator< base_generator_type&, normal_distribution<double> > gaussGen( myRandGen,  normDist );	
			return gaussGen();
		}

		double uniform( void )
		{
			return myUniformGenerator();
		}

		double uniform( double argMin, double argMax )
		{
			return argMin + ( argMax - argMin ) * uniform();
		}

		bool trueWithProb( double argProbability )
		{
			double roll = uniform();
			bool out = ( roll < argProbability );
			//std::cout << "trueWithProb( " << argProbability << " ) => (" << roll << ") -> " << out << "\n";
			return out;
		}

		int uniformInt( int argMin, int argMax )
		{
			double x = uniform( argMin, argMax + 1 );
			return static_cast< int >( x ); // truncates
		}
	private:
		base_generator_type myRandGen;

		boost::normal_distribution < > myNormalDist;
		boost::uniform_real< > myUniformDist;

		boost::variate_generator < base_generator_type&, boost::normal_distribution<> > myNormalGenerator;
		boost::variate_generator < base_generator_type&, boost::uniform_real<> > myUniformGenerator;
	};

	typedef boost::shared_ptr< Entropy > EntropyPtr;
	extern EntropyPtr theEntropy;
}

#endif
