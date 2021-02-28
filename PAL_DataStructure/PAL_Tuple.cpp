#ifndef PAL_TUPLE_CPP
#define PAL_TUPLE_CPP 1

namespace PAL_DS
{
	template <class Ta,class Tb> class Doublet
	{
		public:
			Ta a;
			Tb b;
	
			inline bool operator != (const Doublet &tar) const
			{return a!=tar.a||b!=tar.b;}
			
			inline bool operator == (const Doublet &tar) const
			{return a==tar.a&&b==tar.b;}
			
			inline bool operator < (const Doublet &tar) const
			{
				if (a==tar.a) return b<tar.b;
				else return a<tar.a;
			}
			
			inline bool operator > (const Doublet &tar) const
			{
				if (a==tar.a) return b>tar.b;
				else return a>tar.a;
			}
			
			Doublet(const Ta &_a,const Tb &_b):a(_a),b(_b) {}
			
			Doublet() {}
	};
	
	template <class Ta,class Tb,class Tc> class Triplet
	{
		public:
			Ta a;
			Tb b;
			Tc c;
	
			inline bool operator != (const Triplet &tar) const
			{return a!=tar.a||b!=tar.b||c!=tar.c;}
			
			inline bool operator == (const Triplet &tar) const
			{return a==tar.a&&b==tar.b&&c==tar.c;}
			
			inline bool operator < (const Triplet &tar) const
			{
				if (a==tar.a) 
					if (b==tar.b) return c<tar.c;
					else return b<tar.b;
				else return a<tar.a;
			}
			
			inline bool operator > (const Triplet &tar) const
			{
				if (a==tar.a) 
					if (b==tar.b) return c>tar.c;
					else return b>tar.b;
				else return a>tar.a;
			}
			
			Triplet(const Ta &_a,const Tb &_b,const Tc &_c):a(_a),b(_b),c(_c) {}
			
			Triplet() {}
	};
	
	template <class Ta,class Tb,class Tc,class Td> class Tetrad
	{
		public:
			Ta a;
			Tb b;
			Tc c;
			Td d;
			
			inline bool operator != (const Tetrad &tar) const
			{return a!=tar.a||b!=tar.b||c!=tar.c||d!=tar.d;}
			
			inline bool operator == (const Tetrad &tar) const
			{return a==tar.a&&b==tar.b&&c==tar.c&&d==tar.d;}
			
			inline bool operator < (const Tetrad &tar) const
			{
				if (a==tar.a) 
					if (b==tar.b) 
						if (c==tar.c) return d<tar.d;
						else return c<tar.c;
					else return b<tar.b;
				else return a<tar.a;
			}
			
			inline bool operator > (const Tetrad &tar) const
			{
				if (a==tar.a)
					if (b==tar.b) 
						if (c==tar.c) return d>tar.d;
						else return c>tar.c;
					else return b>tar.b;
				else return a>tar.a;
			}
			
			Tetrad(const Ta &_a,const Tb &_b,const Tc &_c,const Td &_d):a(_a),b(_b),c(_c),d(_d) {}
			
			Tetrad() {}
	};
	
	template <class Ta,class Tb,class Tc,class Td,class Te> class Quintet
	{
		public:
			Ta a;
			Tb b;
			Tc c;
			Td d;
			Te e;
			
			inline bool operator != (const Quintet &tar) const
			{return a!=tar.a||b!=tar.b||c!=tar.c||d!=tar.d||e!=tar.e;}
			
			inline bool operator == (const Quintet &tar) const
			{return a==tar.a&&b==tar.b&&c==tar.c&&d==tar.d&&e==tar.e;}
			
			inline bool operator < (const Quintet &tar) const
			{
				if (a==tar.a) 
					if (b==tar.b) 
						if (c==tar.c) 
							if (d==tar.d) return e<tar.e;
							else return d<tar.d;
						else return c<tar.c;
					else return b<tar.b;
				else return a<tar.a;
			}
			
			inline bool operator > (const Quintet &tar) const
			{
				if (a==tar.a)
					if (b==tar.b) 
						if (c==tar.c) 
							if (d==tar.d) return e>tar.e;
							else return d>tar.d;
						else return c>tar.c;
					else return b>tar.b;
				else return a>tar.a;
			}
			
			Quintet(const Ta &_a,const Tb &_b,const Tc &_c,const Td &_d,const Te &_e):a(_a),b(_b),c(_c),d(_d),e(_e) {}
			
			Quintet() {}
	};
}
#endif
