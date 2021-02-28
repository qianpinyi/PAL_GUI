#ifndef PAL_UNIONFINDSET_CPP
#define PAL_UNIONFINDSET_CPP 1

namespace PAL_DS
{
	class UnionFindSet
	{
		protected:
			int *fa=NULL;
			int Size=0;
			
			int FindFa(int x)
			{
				int re=x,p;
				while (fa[re]!=re) re=fa[re];
				while (fa[x]!=x) p=x,x=fa[x],fa[p]=re;
				return re;
			}
			
		public:
			bool SameSet(int x,int y)
			{return FindFa(x)==FindFa(y);}
			
			bool UnionSet(int x,int y)
			{fa[FindFa(x)]=FindFa(y);}
			
			void Init()
			{
				for (int i=0;i<Size;++i)
					fa[i]=i;
			}
			
			~UnionFindSet()
			{
				delete[] fa;
			}
			
			UnionFindSet(int _size):Size(_size)
			{
				fa=new int[Size];
				Init();
			}
	};
	
	
}

#endif
