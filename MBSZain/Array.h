#ifndef _ARRAY_H_
#define _ARRAY_H_

/*
	warning: this small multidimensional array library uses a few features
	not taught in ENGR112 and not explained in elementary textbooks

	(c) Bjarne Stroustrup, Texas A&M University. 

	Use as you like as long as you acknowledge the source.
*/

#include<string>

struct Array_error {
	std::string name;
	Array_error(char* q) :name(q) { }
	Array_error(std::string n) :name(n) { }
};

inline void error(char* p)
{
	throw Array_error(p);
}

typedef long Index;	// I still dislike unsigned

// The general Array template is simply a prop for its specializations:
template<class T = double, int D = 1> class Array {
	// multidimensional array class
	// ( ) does multidimensional subscripting
	// [ ] does C style "slizing": gives an N-1 dimensional array from an N dimensional one
	// column() is not (yet) implemented because it requires strides.
	// = has copy semantics
	// ( ) and [ ] are range checked
private:
	Array();	// this should never be compiled
};

template<class T = double, int D = 1> class Row ;	// forward declaration

// function objects for various apply() operations:

template<class T> struct Assign {
	void operator()(T& a, const T& c) { a = c; }
};

template<class T> struct Add_assign {
	void operator()(T& a, const T& c) { a += c; }
};
template<class T> struct Mul_assign {
	void operator()(T& a, const T& c) { a *= c; }
};
template<class T> struct Minus_assign {
	void operator()(T& a, const T& c) { a -= c; }
};
template<class T> struct Div_assign {
	void operator()(T& a, const T& c) { a /= c; }
};
template<class T> struct Mod_assign {
	void operator()(T& a, const T& c) { a %= c; }
};
template<class T> struct Or_assign {
	void operator()(T& a, const T& c) { a |= c; }
};
template<class T> struct Xor_assign {
	void operator()(T& a, const T& c) { a ^= c; }
};
template<class T> struct And_assign {
	void operator()(T& a, const T& c) { a &= c; }
};

template<class T> struct Not_assign {
	void operator()(T& a) { a = !a; }
};

template<class T> struct Not {
	T operator()(T& a) { return !a; }
};

template<class T> struct Unary_minus {
	T operator()(T& a) { return -a; }
};

template<class T> struct Complement {
	T operator()(T& a) { return ~a; }
};


// Array_base represents the common part of the Array classes:
template<class T> class Array_base {
	// arrays store their memory (elements) in Array_base and have copy semantics
	// Array_base does element-wise operations
protected:
	T* elem;	// vector? no: we couldn't easily provide a vector for a slice
	const Index sz;	
	mutable bool owns;
	mutable bool xfer;
public:
	Array_base(Index n) :elem(new T[n]()), sz(n), owns(true), xfer(false)
		// array of n elements (default initialized)
	{
//			cerr << "new[" << n << "]->" << elem << "\n";
	}

	Array_base(Index n, T* p) :elem(p), sz(n), owns(false), xfer(false)
		// descriptor for array of n elements owned by someone else
	{
	}

	~Array_base()
	{
			if (owns) {
//					cerr << "delete[" << sz << "] " << elem << "\n";
					delete[]elem;
			}
	}

	// if necessay, we can get to the raw array:
	T* data() { return elem; }
	const T* data() const { return elem; }
	Index size() const { return sz; }

	void copy_elements(const Array_base& a)
	{
		if (sz!=a.sz) error("copy_elements()");
		for (Index i=0; i<sz; ++i) elem[i] = a.elem[i];
	}

	void base_assign(const Array_base& a) { copy_elements(a); }

	void base_copy(const Array_base& a)
	{
		if (a.xfer) {	// a is just about to be deleted
						// so we can transfer ownership rather than copy
//			cerr << "xfer @" << a.elem << " [" << a.sz << "]\n";
			elem = a.elem;
			a.xfer = false;	// note: modifies source
			a.owns = false;
		}
		else {
				elem = new T[a.sz];
//			cerr << "base copy @" << a.elem << " [" << a.sz << "]\n";
				copy_elements(a);
		}
		owns = true;
		xfer = false;
	}

	// to get the elements of a local array out of a function without copying:
	void base_xfer(Array_base& x)
	{
		if (owns==false) error("cannot xfer() non-owner");
		owns = false;	// now the elements are safe from deletion by original owner
		x.xfer = true;	// target asserts temporary ownership
		x.owns = true;
	}

	template<class F> void base_apply(F f) { for (Index i = 0; i<size(); ++i) f(elem[i]); }
	template<class F> void base_apply(F f, const T& c) { for (Index i = 0; i<size(); ++i) f(elem[i],c); }
private:
	void operator=(const Array_base&);	// no ordinary copy of bases
	Array_base(const Array_base&);
};

template<class T> class Array<T,1> : public Array_base<T> {
protected:
	Array(Index n, T* p) :Array_base<T>(n,p) 	// make a slice
	{
//		cerr << "Array1(" << n << "," << p << ")\n";
	}
public:
	Array(Index n) :Array_base<T>(n) { }

	Array(Row<T,1>& a) :Array_base<T>(a.dim1(),a.p) { 
		cerr << "construct Array from Row\n";
	}

	// copy constructor: let the base do the copy:
	Array(const Array& a) :Array_base<T>(a.size(),0)
	{
//		cerr << "copy ctor\n";
		base_copy(a);
	}

	template<class F> Array(const Array& a, F f) : Array_base<T>(a.size(),0)
		// construct an Array with element's that are functions of a's elements
	{
			for (Index i = 0; i<sz; ++i) this->elem[i] = f(a.elem[i]); 
	}

	Array& operator=(const Array& a)
		// copy assignment: let the base do the copy
	{
//		cerr << "copy assignment\n";
		if (this->size() != a.size()) error("length error in =");
		base_assign(a);
		return *this;
	}
	~Array() { }

	Index dim1() const { return sz; }

	Array xfer()	// make an Array to move elements out of a scope
	{
		Array x(this->size(),this->data());	// make a descriptor
		base_xfer(x);			// transfer (temporary) ownership to x
		return x;
	}

	void range_check(Index n) const { if (n<0 || this->sz<=n) error("range error 1"); }

	// subscripting:
	T& operator()(Index n) { range_check(n); return this->elem[n]; }; 
	const T& operator()(Index n) const {  range_check(n); return this->elem[n]; };

	// slicing (the same as subscripting for 1D arrays):
	T& operator[](Index n) { range_check(n); return this->elem[n]; }; 
	const T& operator[](Index n) const { range_check(n); return this->elem[n]; };

	// element-wise operations:
	template<class F> Array& apply(F f) { this->base_apply(f); return *this; }
	template<class F> Array& apply(F f,const T& c) { this->base_apply(f,c); return *this; }

	Array& operator=(const T& c) { this->base_apply(Assign<T>(),c); return *this; }

	Array& operator*=(const T& c) { base_apply(Mul_assign<T>(),c); return *this; }
	Array& operator/=(const T& c) { base_apply(Div_assign<T>(),c); return *this; }
	Array& operator%=(const T& c) { base_apply(Mod_assign<T>(),c); return *this; }
	Array& operator+=(const T& c) { base_apply(Add_assign<T>(),c); return *this; }
	Array& operator-=(const T& c) { base_apply(Minus_assign<T>(),c); return *this; }

	Array& operator&=(const T& c) { base_apply(And_assign<T>(),c); return *this; }
	Array& operator|=(const T& c) { base_apply(Or_assign<T>(),c); return *this; }
	Array& operator^=(const T& c) { base_apply(Xor_assign<T>(),c); return *this; }

	Array operator!() { return xfer(Array(*this,Not<T>())); }
	Array operator-() { return xfer(Array(*this,Unary_minus<T>())); }
	Array operator~() { return xfer(Array(*this,Complement<T>()));  }

	template<class F> Array x(F f) { return xfer(Array(*this,f)); }	// does this have a name?
};

template<class T> Array<T> scale_and_add(const Array<T>& a, T c, const Array<T>& b)
	//  Fortran "saxpy()" ("fma" for "fused multiply-add").
	// will the copy constructor be called twice and defeat the xfer optimization?
{
	if (a.size() != b.size()) error("sizes wrong for scale_and_add()");
	Array<T> res(a.size());
	for (Index i = 0; i<a.size(); ++i) res[i] += a[i]*c+b[i];
	return res.xfer();
}

template<class T> Array<T> dot_product(const Array<T>&a , const Array<T>& b)
{
	if (a.size() != b.size()) error("sizes wrong for dot product");
	T sum = 0;
	for (Index i = 0; i<a.size(); ++i) sum += a[i]*b[i];
	return sum;
}

template<class T, int N> Array<T,N> xfer(Array<T,N>& a)
{
	return a.xfer();
}

template<class T> class Array<T,2> : public Array_base<T> {
	const Index d1;
	const Index d2;

protected:
	// for use by Row:
	Array(Index n1, Index n2, T* p) :Array_base<T>(n1*n2,p), d1(n1), d2(n2)	// make a slice
	{
	}

public:
	Array(Index n1, Index n2) :Array_base<T>(n1*n2), d1(n1), d2(n2) { }

	Array(Row<T,2>& a) :Array_base<T>(a.dim1(),a.dim2(),a.p) { 
		cerr << "construct 2D Array from Row\n";
	}

	Array(const Array& a) :Array_base<T>(a.sz,0), d1(a.d1), d2(a.d2)
	{
		base_copy(a);
	}

	Array& operator=(const Array& a)
	{
		if (d1!=a.d1 || d2!=a.d2) error("length error in 2D =");
		base_assign(a);
		return *this;
	}

	~Array() { }
	
	Index dim1() const { return d1; }	// number of elements in a row
	Index dim2() const { return d2; }	// numbe of elements in a column

	void range_check(Index n1, Index n2) const
	{
//		cerr << "range check: (" << d1 << "," << d2 << "): " << n1 << " " << n2 << "\n";
		if (n1<0 || d1<=n1) error("2D range error: dimension 1");
		if (n2<0 || d2<=n2) error("2D range error: dimension 2");
	}

	// subscripting:
	T& operator()(Index n1, Index n2) { range_check(n1,n2); return this->elem[n1*d2+n2]; } 
	const T& operator()(Index n1, Index n2) const { range_check(n1,n2); return this->elem[n1*d2+n2]; }

	// slicing (return a row):
	Row<T,1> operator[](Index n) { range_check(n,0); return Row<T,1>(d2,&this->elem[n*d2]); };
	const Row<T,1> operator[](Index n) const { range_check(n,0); return Row<T,1>(d2,&this->elem[n*d2]); }

	Row<T,1> row(Index n) { range_check(n,0); return Row<T,1>(d2,&this->elem[n*d2]); }; 
	const Row<T,1> row(Index n) const { range_check(n,0); return Row<T,1>(d2,&this->elem[n*d2]); }

	// Column<T,1> column(Index n); // not yet implemented: requies strides


	// element-wise operations:
	template<class F> Array& apply(F f) { this->base_apply(f); return *this; }
	template<class F> Array& apply(F f,const T& c) { this->base_apply(f,c); return *this; }

	Array& operator=(const T& c) { this->base_apply(Assign<T>(),c); return *this; }

	Array& operator*=(const T& c) { base_apply(Mul_assign<T>(),c); return *this; }
	Array& operator/=(const T& c) { base_apply(Div_assign<T>(),c); return *this; }
	Array& operator%=(const T& c) { base_apply(Mod_assign<T>(),c); return *this; }
	Array& operator+=(const T& c) { base_apply(Add_assign<T>(),c); return *this; }
	Array& operator-=(const T& c) { base_apply(Minus_assign<T>(),c); return *this; }

	Array& operator&=(const T& c) { base_apply(And_assign<T>(),c); return *this; }
	Array& operator|=(const T& c) { base_apply(or_assign<T>(),c); return *this; }
	Array& operator^=(const T& c) { base_apply(Xor_assign<T>(),c); return *this; }

	Array operator!() { return xfer(Array(*this,Not<T>())); }
	Array operator-() { return xfer(Array(*this,Unary_minus<T>())); }
	Array operator~() { return xfer(Array(*this,Complement<T>()));  }

	template<class F> Array apply_new(F f) { return xfer(Array(*this,f)); }	

	void swap_rows(Index i, Index j)
		// swap_rows() uses a row's worth of memory for better run-time performance
		// if you want pairwise swap, just write it yourself
	{
		if (i == j) return;
	//	Array<T,1> temp(d2);
	//	temp = (*this)[i];
		Array<T,1> temp = (*this)[i];
		(*this)[i] = (*this)[j];
		(*this)[j] = temp;
	}
};

template<class T = double, int D = 1> class Row {
	// general version exists only to allow specializations
private:
		Row();
};

template<class T> class Row<T,1> : public Array<T,1> {
public:
	Row(Index n, T* p) : Array<T,1>(n,p) 	// make a slice
	{
//		cerr << "Row(" << n << "," << p << ")\n";
	}

	Array<T,1>& operator=(const Array<T,1>& a)
	{
		return *static_cast<Array<T,1>*>(this)=a;
	}
};


template<class T> class Row<T,2> : public Array<T,2> {
public:
	Row(Index n1, Index n2, T* p) :Array<T,2>(n1,n2,p)
	{
	}

	Array<T,2>& operator=(const Array<T,2>& a)
	{
		return *static_cast<Array<T,2>*>(this)=a;
	}
};

template<class T, int N> Array<T,N-1> scale_and_add(const Array<T,N>& a, const Array<T,N-1> c, const Array<T,N-1>& b)
{
	Array<T> res(a.size());
	if (a.size() != b.size()) error("sizes wrong for scale_and_add");
	for (Index i = 0; i<a.size(); ++i) res[i] += a[i]*c+b[i];
	return res.xfer();
}

template<class T> class Array<T,3> : public Array_base<T> {
	const Index d1;
	const Index d2;
	const Index d3;
public:
	Array(Index n1, Index n2, Index n3) :Array_base<T>(n1*n2*n3), d1(n1), d2(n2), d3(n3) { }

	Array(Index n1, Index n2, Index n3, T* p) :Array_base<T>(n1*n2*n3,p), d1(n1), d2(n2), d3(n3)	// make a slice
	{
	}

	Array(const Array& a) :Array_base<T>(a.sz,0), d1(a.d1), d2(a.d2), d3(a.d3)
	{
		base_copy(a);
	}

	Array& operator=(const Array& a)
	{
		if (d1!=a.d1 || d2!=a.d2 || d3!=a.d3) error("length error in 2D =");
		base_assign(a);
		return *this;
	}

	~Array() { }

	int dim1() const { return d1; }
	int dim2() const { return d2; }
	int dim3() const { return d3; }

	Array xfer()	// make an Array to move elements out of a scope
	{
		Array x(this->size(),this->data());	// make a descriptor
		base_xfer(x);			// transfer (temporary) ownership to x
		return x;
	}

	void range_check(Index n1, Index n2, Index n3) const
	{
		if (n1<0 || d1<=n1) error("3D range error: dimension 1");
		if (n2<0 || d2<=n2) error("3D range error: dimension 2");
		if (n3<0 || d3<=n3) error("3D range error: dimension 3");
	}

	T& operator()(Index n1, Index n2, Index n3) { range_check(n1,n2,n3); return this->elem[d2*d3*n1+d3*n2+n3]; }; 
	const T& operator()(Index n1, Index n2, Index n3) const { range_check(n1,n2,n3); return this->elem[d2*d3*n1+d3*n2+n3]; };

	// Slice:
	Row<T,2> row(Index n) { range_check(n,0,0); return Row<T,2>(d2,d3,&this->elem[n*d2*d3]); }
	const Row<T,2> row(Index n) const { range_check(n,0,0); return Row<T,2>(d2,d3,&this->elem[n*d2*d3]); }

	Row<T,2> operator[](Index n) { return row(n); } 
	const Row<T,2> operator[](Index n) const { return row(n); }

	// Column<T,2> column(Index n);	// not (yet) implemented

	
	// element-wise operations:
	template<class F> Array& apply(F f) { this->base_apply(f); return *this; }
	template<class F> Array& apply(F f,const T& c) { this->base_apply(f,c); return *this; }

	Array& operator=(const T& c) { this->base_apply(Assign<T>(),c); return *this; }

	Array& operator*=(const T& c) { base_apply(Mul_assign<T>(),c); return *this; }
	Array& operator/=(const T& c) { base_apply(Div_assign<T>(),c); return *this; }
	Array& operator%=(const T& c) { base_apply(Mod_assign<T>(),c); return *this; }
	Array& operator+=(const T& c) { base_apply(Add_assign<T>(),c); return *this; }
	Array& operator-=(const T& c) { base_apply(Minus_assign<T>(),c); return *this; }

	Array& operator&=(const T& c) { base_apply(And_assign<T>(),c); return *this; }
	Array& operator|=(const T& c) { base_apply(or_assign<T>(),c); return *this; }
	Array& operator^=(const T& c) { base_apply(Xor_assign<T>(),c); return *this; }

	Array operator!() { return xfer(Array(*this,Not<T>())); }
	Array operator-() { return xfer(Array(*this,Unary_minus<T>())); }
	Array operator~() { return xfer(Array(*this,Complement<T>()));  }

	template<class F> Array apply_new(F f) { return xfer(Array(*this,f)); }
	
	void swap_rows(Index i, Index j)
		// swap_rows() uses a row's worth of memory for better run-time performance
		// if you want pairwise swap, just write it yourself
	{
		if (i == j) return;
		//Array<T,2> temp(d2);
		//temp = (*this)[i];
		Array<T,2> temp = (*this)[i];
		(*this)[i] = (*this)[j];
		(*this)[j] = temp;
	}
};

#endif