/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 1998,2002  Steve Baker
 
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
 
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
 
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 
     For further information visit http://plib.sourceforge.net

     $Id: ssgaCloudLayer.cxx 1793 2003-08-26 21:36:53Z nmcevoy $
*/

// Written by Curtis Olson, started June 2000.
//
// Moved into ssgAux, July 2003.

#include "plib_ssgaSky.h"

/* Nautical Miles to Meters */
#define SG_NM_TO_METER  1852.0000

/* Radians to Nautical Miles.  1 nm = 1/60 of a degree */
#define SG_NM_TO_RAD    0.00029088820866572159

/* Nautical Miles to Radians */
#define SG_RAD_TO_NM    3437.7467707849392526

/* For divide by zero avoidance, this will be close enough to zero */
#define SG_EPSILON      0.0000001

void calc_gc_course_dist( const sgVec2& start, const sgVec2& dest, double *course, double *dist );

// make an ssgSimpleState for a cloud layer given the named texture
ssgSimpleState *sgCloudMakeState( const char* path );


ssgaCloudLayer::ssgaCloudLayer( void ) :
  layer_root(new ssgRoot),
  layer_transform(new ssgTransform),
  enabled(true),
  layer_span(0.0),
  layer_asl(0.0),
  layer_thickness(0.0),
  layer_transition(0.0),
  scale(4000.0),
  speed(0.0),
  direction(0.0),
  last_lon(0.0),
  last_lat(0.0),
  last_x(0.0),
  last_y(0.0)
{
  cl[0] = cl[1] = cl[2] = cl[3] = NULL;
  vl[0] = vl[1] = vl[2] = vl[3] = NULL;
  tl[0] = tl[1] = tl[2] = tl[3] = NULL;
  layer[0] = layer[1] = layer[2] = layer[3] = NULL;

  layer_root->addKid(layer_transform);
}

  
ssgaCloudLayer::~ssgaCloudLayer()
{
  delete layer_root; // deletes layer_transform and layer as well
}

  
void
ssgaCloudLayer::build( const char *cloud_tex_path, float span, float elevation, float thickness, float transition )
{
  ssgSimpleState *cloud_state = sgCloudMakeState( cloud_tex_path );
  build(cloud_state, span, elevation, thickness, transition);
}


void
ssgaCloudLayer::build( ssgSimpleState *cloud_state, float span, float elevation, float thickness, float transition )
{
  layer_span = span;
  layer_asl = elevation;
  layer_thickness = thickness;
  layer_transition = transition;

  scale = 4000.0;
  last_lon = last_lat = -999.0f;
  last_x = last_y = 0.0f;

  sgVec2 base;
  sgSetVec2( base, (float)ssgaRandom(), (float)ssgaRandom() );

  // build the cloud layer
  sgVec4 color;
  sgVec3 vertex;
  sgVec2 tc;

  const float layer_scale = layer_span / scale;
  const float mpi = SG_PI/4;
  const float alt_diff = layer_asl * 1.5f;

  for (int i = 0; i < 4; i++) {
    if ( layer[i] != NULL ) {
      layer_transform->removeKid(layer[i]); // automatic delete
	}

    vl[i] = new ssgVertexArray( 10 );
    cl[i] = new ssgColourArray( 10 );
    tl[i] = new ssgTexCoordArray( 10 );

    sgSetVec3( vertex, layer_span*(i-2)/2, -layer_span,
      (float)(alt_diff * (sin(i*mpi) - 2)) );

    sgSetVec2( tc, base[0] + layer_scale * i/4, base[1] );

    sgSetVec4( color, 1.0f, 1.0f, 1.0f, (i == 0) ? 0.0f : 0.15f );

    cl[i]->add( color );
    vl[i]->add( vertex );
    tl[i]->add( tc );

    for (int j = 0; j < 4; j++) {

      sgSetVec3( vertex, layer_span*(i-1)/2, layer_span*(j-2)/2,
        (float)(alt_diff * (sin((i+1)*mpi) + sin(j*mpi) - 2)) );

      sgSetVec2( tc, base[0] + layer_scale * (i+1)/4,
        base[1] + layer_scale * j/4 );

      sgSetVec4( color, 1.0f, 1.0f, 1.0f,
        ( (j == 0) || (i == 3)) ?  
        ( (j == 0) && (i == 3)) ? 0.0f : 0.15f : 1.0f );

      cl[i]->add( color );
      vl[i]->add( vertex );
      tl[i]->add( tc );

      sgSetVec3( vertex, layer_span*(i-2)/2, layer_span*(j-1)/2,
        (float)(alt_diff * (sin(i*mpi) + sin((j+1)*mpi) - 2)) );

      sgSetVec2( tc, base[0] + layer_scale * i/4,
        base[1] + layer_scale * (j+1)/4 );

      sgSetVec4( color, 1.0f, 1.0f, 1.0f,
        ((j == 3) || (i == 0)) ?
        ((j == 3) && (i == 0)) ? 0.0f : 0.15f : 1.0f );
      cl[i]->add( color );
      vl[i]->add( vertex );
      tl[i]->add( tc );
    }

    sgSetVec3( vertex, layer_span*(i-1)/2, layer_span, 
      (float)(alt_diff * (sin((i+1)*mpi) - 2)) );

    sgSetVec2( tc, base[0] + layer_scale * (i+1)/4,
    base[1] + layer_scale );

    sgSetVec4( color, 1.0f, 1.0f, 1.0f, (i == 3) ? 0.0f : 0.15f );

    cl[i]->add( color );
    vl[i]->add( vertex );
    tl[i]->add( tc );

    layer[i] = new ssgVtxTable(GL_TRIANGLE_STRIP, vl[i], NULL, tl[i], cl[i]);
    layer_transform->addKid( layer[i] );

    layer[i]->setState( cloud_state );
  }

  // force a repaint of the sky colors with arbitrary defaults
  repaint( color );
}


bool ssgaCloudLayer::repositionFlat( sgVec3 p, double dt )
{
  sgMat4 T1;

  // combine p and asl (meters) to get translation offset
  sgVec3 asl_offset;
  if ( p[SG_Z] <= layer_asl ) {
    sgSetVec3( asl_offset, p[SG_X], p[SG_Y], layer_asl );
  }
  else {
    sgSetVec3( asl_offset, p[SG_X], p[SG_Y], layer_asl + layer_thickness );
  }

  // Translate to elevation
  sgMakeTransMat4( T1, asl_offset );

  sgMat4 TRANSFORM;
  sgCopyMat4( TRANSFORM, T1 );

  sgCoord layerpos;
  sgSetCoord( &layerpos, TRANSFORM );

  layer_transform->setTransform( &layerpos );

  // now calculate update texture coordinates
  double sp_dist = speed*dt;

  if ( p[SG_X] != last_x || p[SG_Y] != last_y || sp_dist != 0 ) {

    // calculate cloud movement
    double ax = 0.0, ay = 0.0, bx = 0.0, by = 0.0;

    ax = p[SG_X] - last_x;
    ay = p[SG_Y] - last_y;

    if (sp_dist > 0) {
      bx = cos(-direction * SGD_DEGREES_TO_RADIANS) * sp_dist;
      by = sin(-direction * SGD_DEGREES_TO_RADIANS) * sp_dist;
    }

    float xoff = (float)((ax + bx) / (2 * scale));
    float yoff = (float)((ay + by) / (2 * scale));

    const float layer_scale = layer_span / scale;

    float *base, *tc;

    base = tl[0]->get( 0 );
    base[0] += xoff;

    // the while loops can lead to *long* pauses if base[0] comes
    // with a bogus value.
    // while ( base[0] > 1.0 ) { base[0] -= 1.0; }
    // while ( base[0] < 0.0 ) { base[0] += 1.0; }
    if ( base[0] > -10.0 && base[0] < 10.0 ) {
      base[0] -= (int)base[0];
    }
	else {
      base[0] = 0.0;
	  ulSetError(UL_WARNING, "Warning: base1\n");
    }

    base[1] += yoff;
    // the while loops can lead to *long* pauses if base[0] comes
    // with a bogus value.
    // while ( base[1] > 1.0 ) { base[1] -= 1.0; }
    // while ( base[1] < 0.0 ) { base[1] += 1.0; }
    if ( base[1] > -10.0 && base[1] < 10.0 ) {
      base[1] -= (int)base[1];
    }
	else {
      base[1] = 0.0;
	  ulSetError(UL_WARNING, "Warning: base2\n");
    }

    for (int i = 0; i < 4; i++) {
      tc = tl[i]->get( 0 );
      sgSetVec2( tc, base[0] + layer_scale * i/4, base[1] );

      for (int j = 0; j < 4; j++) {
        tc = tl[i]->get( j*2+1 );
        sgSetVec2( tc, base[0] + layer_scale * (i+1)/4,
        base[1] + layer_scale * j/4 );

        tc = tl[i]->get( (j+1)*2 );
        sgSetVec2( tc, base[0] + layer_scale * i/4,
        base[1] + layer_scale * (j+1)/4 );
	  }

      tc = tl[i]->get( 9 );
      sgSetVec2( tc, base[0] + layer_scale * (i+1)/4,
      base[1] + layer_scale );
	}

    last_x = p[SG_X];
    last_y = p[SG_Y];
  }

  return true;
}


bool ssgaCloudLayer::reposition( sgVec3 p, sgVec3 up, double lon, double lat, double alt, double dt )
{
  sgMat4 T1, LON, LAT;
  sgVec3 axis;

  // combine p and asl (meters) to get translation offset
  sgVec3 asl_offset;
  sgCopyVec3( asl_offset, up );
  sgNormalizeVec3( asl_offset );
  if ( alt <= layer_asl ) {
    sgScaleVec3( asl_offset, layer_asl );
  }
  else {
    sgScaleVec3( asl_offset, layer_asl + layer_thickness );
  }
  sgAddVec3( asl_offset, p );

  // Translate to zero elevation
  sgMakeTransMat4( T1, asl_offset );

  // Rotate to proper orientation
  sgSetVec3( axis, 0.0, 0.0, 1.0 );
  sgMakeRotMat4( LON, (float)(lon * SGD_RADIANS_TO_DEGREES), axis );

  sgSetVec3( axis, 0.0, 1.0, 0.0 );
  sgMakeRotMat4( LAT, (float)(90.0 - lat * SGD_RADIANS_TO_DEGREES), axis );

  sgMat4 TRANSFORM;

  sgCopyMat4( TRANSFORM, T1 );
  sgPreMultMat4( TRANSFORM, LON );
  sgPreMultMat4( TRANSFORM, LAT );

  sgCoord layerpos;
  sgSetCoord( &layerpos, TRANSFORM );

  layer_transform->setTransform( &layerpos );

  // now calculate update texture coordinates
  if ( last_lon < -900 ) {
    last_lon = lon;
    last_lat = lat;
  }

  double sp_dist = speed*dt;

  if ( lon != last_lon || lat != last_lat || sp_dist != 0 ) {

    double course = 0.0, dist = 0.0;
	if ( lon != last_lon || lat != last_lat ) {
	    sgVec2 start, dest;
	    sgSetVec2(start, (float)last_lon, (float)last_lat);
	    sgSetVec2(dest, (float)lon, (float)lat);
		calc_gc_course_dist( dest, start, &course, &dist );
	}

    // calculate cloud movement
    double ax = 0.0, ay = 0.0, bx = 0.0, by = 0.0;

    if (dist > 0.0) {
      ax = cos(course) * dist;
      ay = sin(course) * dist;
    }

    if (sp_dist > 0) {
      bx = cos(-direction * SGD_DEGREES_TO_RADIANS) * sp_dist;
      by = sin(-direction * SGD_DEGREES_TO_RADIANS) * sp_dist;
    }

    float xoff = (float)((ax + bx) / (2 * scale));
    float yoff = (float)((ay + by) / (2 * scale));

    const float layer_scale = layer_span / scale;

    float *base, *tc;

    base = tl[0]->get( 0 );
    base[0] += xoff;

    // the while loops can lead to *long* pauses if base[0] comes
    // with a bogus value.
    // while ( base[0] > 1.0 ) { base[0] -= 1.0; }
    // while ( base[0] < 0.0 ) { base[0] += 1.0; }
    if ( base[0] > -10.0 && base[0] < 10.0 ) {
      base[0] -= (int)base[0];
    }
	else {
      base[0] = 0.0;
	  ulSetError(UL_WARNING, "Warning: base1\n");
    }

    base[1] += yoff;
    // the while loops can lead to *long* pauses if base[0] comes
    // with a bogus value.
    // while ( base[1] > 1.0 ) { base[1] -= 1.0; }
    // while ( base[1] < 0.0 ) { base[1] += 1.0; }
    if ( base[1] > -10.0 && base[1] < 10.0 ) {
      base[1] -= (int)base[1];
    }
	else {
      base[1] = 0.0;
	  ulSetError(UL_WARNING, "Warning: base2\n");
    }

    for (int i = 0; i < 4; i++) {
      tc = tl[i]->get( 0 );
      sgSetVec2( tc, base[0] + layer_scale * i/4, base[1] );

      for (int j = 0; j < 4; j++) {
        tc = tl[i]->get( j*2+1 );
        sgSetVec2( tc, base[0] + layer_scale * (i+1)/4,
        base[1] + layer_scale * j/4 );

        tc = tl[i]->get( (j+1)*2 );
        sgSetVec2( tc, base[0] + layer_scale * i/4,
        base[1] + layer_scale * (j+1)/4 );
	  }

      tc = tl[i]->get( 9 );
      sgSetVec2( tc, base[0] + layer_scale * (i+1)/4,
      base[1] + layer_scale );
	}

    last_lon = lon;
    last_lat = lat;
  }

  return true;
}


bool ssgaCloudLayer::repaint( sgVec3 fog_color )
{
  float *color;

  for ( int i = 0; i < 4; i++ )
    for ( int j = 0; j < 10; ++j ) {
      color = cl[i]->get( j );
      sgCopyVec3( color, fog_color );
    }

  return true;
}


void ssgaCloudLayer::draw()
{
  if (!enabled)
	  return;

  ssgCullAndDraw( layer_root );
}


ssgSimpleState *sgCloudMakeState( const char* path )
{
  ssgSimpleState *state = new ssgSimpleState();

  state->setTexture( path );
  state->setShadeModel( GL_SMOOTH );
  state->disable( GL_LIGHTING );
  state->disable( GL_CULL_FACE );
  state->enable( GL_TEXTURE_2D );
  state->enable( GL_COLOR_MATERIAL );
  state->setColourMaterial( GL_AMBIENT_AND_DIFFUSE );
  state->setMaterial( GL_EMISSION, 0, 0, 0, 1 );
  state->setMaterial( GL_SPECULAR, 0, 0, 0, 1 );
  state->enable( GL_BLEND );
  state->enable( GL_ALPHA_TEST );
  state->setAlphaClamp( 0.01f );

  return state;
}


void calc_gc_course_dist( const sgVec2& start, const sgVec2& dest, double *course, double *dist )
{
    // d = 2*asin(sqrt((sin((lat1-lat2)/2))^2 + 
    //            cos(lat1)*cos(lat2)*(sin((lon1-lon2)/2))^2))
    double cos_start_y = cos( start[SG_Y] );
    volatile double tmp1 = sin( (start[SG_Y] - dest[SG_Y]) * 0.5 );
    volatile double tmp2 = sin( (start[SG_X] - dest[SG_X]) * 0.5 );
    double d = 2.0 * asin( sqrt( tmp1 * tmp1 + 
                                 cos_start_y * cos(dest[SG_Y]) * tmp2 * tmp2));

    *dist = d * SG_RAD_TO_NM * SG_NM_TO_METER;

    // We obtain the initial course, tc1, (at point 1) from point 1 to
    // point 2 by the following. The formula fails if the initial
    // point is a pole. We can special case this with:
    //
    // IF (cos(lat1) < EPS)   // EPS a small number ~ machine precision
    //   IF (lat1 > 0)
    //     tc1= pi        //  starting from N pole
    //   ELSE
    //     tc1= 0         //  starting from S pole
    //   ENDIF
    // ENDIF
    //
    // For starting points other than the poles: 
    // 
    // IF sin(lon2-lon1)<0       
    //   tc1=acos((sin(lat2)-sin(lat1)*cos(d))/(sin(d)*cos(lat1)))    
    // ELSE       
    //   tc1=2*pi-acos((sin(lat2)-sin(lat1)*cos(d))/(sin(d)*cos(lat1)))    
    // ENDIF 

    // if ( cos(start[SG_Y]) < SG_EPSILON ) {
    // doing it this way saves a transcendental call
    double sin_start_y = sin( start[SG_Y] );
    if ( fabs(1.0-sin_start_y) < SG_EPSILON ) {
        // EPS a small number ~ machine precision
        if ( start[SG_Y] > 0 ) {
            *course = SGD_PI;   // starting from N pole
        } else {
            *course = 0;        // starting from S pole
        }
    } else {
        // For starting points other than the poles: 
        // double tmp3 = sin(d)*cos_start_y);
        // double tmp4 = sin(dest[SG_Y])-sin(start[SG_Y])*cos(d);
        // double tmp5 = acos(tmp4/tmp3);

        // Doing this way gaurentees that the temps are
        // not stored into memory
        double tmp5 = acos( (sin(dest[SG_Y]) - sin_start_y * cos(d)) /
                            (sin(d) * cos_start_y) );

        // if ( sin( dest[SG_X] - start[SG_X] ) < 0 ) {
        // the sin of the negative angle is just the opposite sign
        // of the sin of the angle  so tmp2 will have the opposite
        // sign of sin( dest[SG_X] - start[SG_X] )
        if ( tmp2 >= 0 ) {
            *course = tmp5;
        } else {
            *course = 2 * SGD_PI - tmp5;
        }
    }
}
