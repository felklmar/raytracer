#include "BoundingBox.h"
#include <limits>

BoundingBox::BoundingBox() 
    : m_Min( std::numeric_limits<float>::infinity() ), 
      m_Max( -std::numeric_limits<float>::infinity() ) {} 

BoundingBox::BoundingBox( const vec3 & min, const vec3 & max )
    : m_Min( min ), m_Max( max ) {}

BoundingBox::BoundingBox( const std::pair<vec3, vec3> & minmax )
    : m_Min( minmax.first ), m_Max( minmax.second ) {}

bool BoundingBox::isValid() const { 
    return m_Min.x <= m_Max.x && m_Min.y <= m_Max.y && m_Min.z <= m_Max.z; 
}

BoundingBox & BoundingBox::expand( const vec3 & point ) {
    m_Min.x = std::min( m_Min.x, point.x );
    m_Min.y = std::min( m_Min.y, point.y );
    m_Min.z = std::min( m_Min.z, point.z );

    m_Max.x = std::max( m_Max.x, point.x );
    m_Max.y = std::max( m_Max.y, point.y );
    m_Max.z = std::max( m_Max.z, point.z );
    return *this;
}

void BoundingBox::expand( const BoundingBox & other ) {
    expand( other.m_Min ).expand( other.m_Max );
}

vec3 BoundingBox::center() const { 
    return ( m_Min + m_Max ) * 0.5f; 
}

vec3 BoundingBox::dimensions() const { 
    return m_Max - m_Min; 
}

vec3 BoundingBox::getMin() const {
    return m_Min;
}

vec3 BoundingBox::getMax() const {
    return m_Max;
}

bool BoundingBox::intersects( const BoundingBox & other ) const {
    return ( m_Min.x <= other.m_Max.x && m_Max.x >= other.m_Min.x ) &&
           ( m_Min.y <= other.m_Max.y && m_Max.y >= other.m_Min.y ) &&
           ( m_Min.z <= other.m_Max.z && m_Max.z >= other.m_Min.z );
}

/*
bool BoundingBox::intersects( const Triangle & t ) const {
    BoundingBox tBounds( t.getBounds() );

    std::cout << "A ";

    if ( !intersects( tBounds ) ) return false;

    std::cout << "B ";

    vec3 n = t.getNormal();

    vec3 p = m_Min;
    vec3 dp = m_Max - m_Min;

    auto c = vec3( 
        n.x > 0.0 ? dp.x : 0.0,
        n.y > 0.0 ? dp.y : 0.0,
        n.z > 0.0 ? dp.z : 0.0 );

    double d1 = n.dot( c - t.getA() );
    double d2 = n.dot( dp - c - t.getA() );

    if ( ( n.dot( p ) + d1 ) * ( n.dot( p ) + d2 ) > 0.0 )
        return false;

    std::cout << "C ";

    auto xym = ( n.z < 0.0 ? -1.0 : 1.0 );
    vec3 neAxy = vec3( -t.getAB().y, t.getAB().x, 0.0 ) * xym; 
    vec3 neBxy = vec3( -t.getBC().y, t.getBC().x, 0.0 ) * xym;
    vec3 neCxy = vec3( -t.getCA().y, t.getCA().x, 0.0 ) * xym;

    vec3 Axy = vec3( t.getA().x, t.getA().y, 0.0 );
    vec3 Bxy = vec3( t.getB().x, t.getB().y, 0.0 );
    vec3 Cxy = vec3( t.getC().x, t.getC().y, 0.0 );

    double deAxy = -neAxy.dot( Axy ) + std::max( 0.0, dp.x * neAxy.x ) + std::max( 0.0, dp.y * neAxy.y );
    double deBxy = -neBxy.dot( Bxy ) + std::max( 0.0, dp.x * neBxy.x ) + std::max( 0.0, dp.y * neBxy.y );
    double deCxy = -neCxy.dot( Cxy ) + std::max( 0.0, dp.x * neCxy.x ) + std::max( 0.0, dp.y * neCxy.y );

    vec3 pxy = vec3( p.x, p.y, 0.0 );

    if ( ( neAxy.dot( pxy ) + deAxy ) < 0.0 || ( neBxy.dot( pxy ) + deBxy ) < 0.0 || ( neCxy.dot( pxy ) + deCxy ) < 0.0 )
        return false;

    std::cout << "D ";

    auto yzm = ( n.x < 0.0 ? -1.0 : 1.0 );
    vec3 neAyz = vec3( -t.getAB().z, t.getAB().y, 0.0 ) * yzm; 
    vec3 neByz = vec3( -t.getBC().z, t.getBC().y, 0.0 ) * yzm;
    vec3 neCyz = vec3( -t.getCA().z, t.getCA().y, 0.0 ) * yzm;

    vec3 Ayz = vec3( t.getA().y, t.getA().z, 0.0 );
    vec3 Byz = vec3( t.getB().y, t.getB().z, 0.0 );
    vec3 Cyz = vec3( t.getC().y, t.getC().z, 0.0 );

    double deAyz = -neAyz.dot( Ayz ) + std::max( 0.0, dp.y * neAyz.x ) + std::max( 0.0, dp.z * neAyz.y );
    double deByz = -neByz.dot( Byz ) + std::max( 0.0, dp.y * neByz.x ) + std::max( 0.0, dp.z * neByz.y );
    double deCyz = -neCyz.dot( Cyz ) + std::max( 0.0, dp.y * neCyz.x ) + std::max( 0.0, dp.z * neCyz.y );

    vec3 pyz = vec3( p.y, p.z, 0.0 );

    if ( ( neAyz.dot( pyz ) + deAyz ) < 0.0 || ( neByz.dot( pyz ) + deByz ) < 0.0 || ( neCyz.dot( pyz ) + deCyz ) < 0.0 )
        return false;

    std::cout << "E ";

    auto zxm = ( n.y < 0.0 ? -1.0 : 1.0 );
    vec3 neAzx = ( vec3( -t.getAB().x, t.getAB().z, 0.0 ) * zxm ).normalize(); 
    vec3 neBzx = ( vec3( -t.getBC().x, t.getBC().z, 0.0 ) * zxm ).normalize();
    vec3 neCzx = ( vec3( -t.getCA().x, t.getCA().z, 0.0 ) * zxm ).normalize();

    vec3 Azx = vec3( t.getA().z, t.getA().x, 0.0 );
    vec3 Bzx = vec3( t.getB().z, t.getB().x, 0.0 );
    vec3 Czx = vec3( t.getC().z, t.getC().x, 0.0 );

    double deAzx = -neAzx.dot( Azx ) + std::max( 0.0, dp.y * neAzx.x ) + std::max( 0.0, dp.z * neAzx.y );
    double deBzx = -neBzx.dot( Bzx ) + std::max( 0.0, dp.y * neBzx.x ) + std::max( 0.0, dp.z * neBzx.y );
    double deCzx = -neCzx.dot( Czx ) + std::max( 0.0, dp.y * neCzx.x ) + std::max( 0.0, dp.z * neCzx.y );

    vec3 pzx = vec3( p.z, p.x, 0.0 );
   
    std::cout << "n.y = " << n.y << "; zxm = " << zxm << std::endl;
    std::cout << "neAzx = " << neAzx << "; neBzx = " << neBzx << "; neCzx = " << neCzx << std::endl;
    std::cout << "Azx = " << Azx << "; Bzx = " << Bzx << "; Czx = " << Czx << std::endl;
    std::cout << "deAzx = " << deAzx << "; deBzx = " << deBzx << "; deCzx = " << deCzx << std::endl;
    std::cout << "pzx = " << pzx << std::endl;
    std::cout << "neAzx.dot( pzx ) + deAzx = " << neAzx.dot( pzx ) + deAzx << std::endl;
    std::cout << "neBzx.dot( pzx ) + deBzx = " << neBzx.dot( pzx ) + deBzx << std::endl;
    std::cout << "neCzx.dot( pzx ) + deCzx = " << neCzx.dot( pzx ) + deCzx << std::endl;

    if ( ( neAzx.dot( pzx ) + deAzx ) < 0.0 || ( neBzx.dot( pzx ) + deBzx ) < 0.0 || ( neCzx.dot( pzx ) + deCzx ) < 0.0 )
        return false;

    std::cout << "F";
    return true;
}
*/

std::pair<double,double> BoundingBox::project( const std::vector<vec3> & points, const vec3 & axis ) const {
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    for ( const auto & p : points ) {
        double val = axis.dot( p );
        if ( val < min ) min = val;
        if ( val > max ) max = val;
    }
    return std::make_pair( min, max );
}

std::vector<vec3> BoundingBox::getVertices() const {
    std::vector<vec3> vertices;
    vertices.push_back( m_Min );
    vertices.push_back( m_Max );
    vertices.push_back( vec3( m_Min.x, m_Min.y, m_Max.z ) );
    vertices.push_back( vec3( m_Min.x, m_Max.y, m_Min.z ) );
    vertices.push_back( vec3( m_Max.x, m_Min.y, m_Min.z ) );
    vertices.push_back( vec3( m_Min.x, m_Max.y, m_Max.z ) );
    vertices.push_back( vec3( m_Max.x, m_Min.y, m_Max.z ) );
    vertices.push_back( vec3( m_Max.x, m_Max.y, m_Min.z ) );
    return vertices;
}

bool BoundingBox::intersects( const Triangle & t ) const {
    std::vector<vec3> boxNormals = {
        vec3( 1.0, 0.0, 0.0 ),
        vec3( 0.0, 1.0, 0.0 ),
        vec3( 0.0, 0.0, 1.0 )
    };

    std::vector<vec3> triangleVertices = {
        t.getA(), t.getB(), t.getC()  
    };

    for ( const auto & normal : boxNormals ) {        
        auto triangleProjection = project( triangleVertices, normal );
        if ( ( triangleProjection.second < m_Min.x && normal.x == 1 ) || ( triangleProjection.first > m_Max.x && normal.x == 1 ) ||
             ( triangleProjection.second < m_Min.y && normal.y == 1 ) || ( triangleProjection.first > m_Max.y && normal.y == 1 ) ||
             ( triangleProjection.second < m_Min.z && normal.z == 1 ) || ( triangleProjection.first > m_Max.z && normal.z == 1 ) )
            return false;
    }

    vec3 triangleNormal = t.getNormal();
    double triangleOffset = triangleNormal.dot( t.getA() );
    auto boxProjection = project( getVertices(), triangleNormal );
    if ( boxProjection.second < triangleOffset || boxProjection.first > triangleOffset )
        return false;
    
    std::vector<vec3> triangleEdges = {
        t.getB() - t.getA(),
        t.getC() - t.getB(),
        t.getA() - t.getC()
    };

    for ( const auto & edge : triangleEdges ) {
        for ( const auto & normal : boxNormals ) {
            vec3 axis = edge.cross( normal ).normalize();
            if ( axis.length() * axis.length() < 1e-9 ) continue;

            auto boxProj = project( getVertices(), axis );
            auto triangleProj = project( triangleVertices, axis );
            if ( boxProj.second < triangleProj.first || boxProj.first > triangleProj.second )
                return false;
        }
    }

    return true;
}

std::ostream & operator<<( std::ostream & out, const BoundingBox & box ) {
    out << box.m_Min << " - " << box.m_Max;
    return out;
}
