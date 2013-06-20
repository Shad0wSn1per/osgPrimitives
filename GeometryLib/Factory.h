#include "Singleton.h"
#include "IProcedural.h"
//#include "Geometry.h"

namespace Utility
{
	namespace GeometryFactory
	{
		class Loft;
		struct IGeometry2D;
		struct IGeometry3D;
		struct ILoft;
		

		using namespace Utility::GeometryFactory;
		
		class Factory: public Singleton< Factory >, public IFactory
		{
		public:
			
			virtual Utility::GeometryFactory::IGeometry2D *Geometry2D();
			virtual IGeometry3D *Geometry3D();
			virtual ILoft *Loft( osg::Group * g );
			~Factory();
		private:
			std::vector< Utility::GeometryFactory::Loft* > m_LoftObjects;
		};
	}
}