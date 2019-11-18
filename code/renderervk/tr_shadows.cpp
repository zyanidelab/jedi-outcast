// leave this as first line for PCH reasons...
//
#include "../server/exe_headers.h"


#include "tr_local.h"


/*

  for a projection shadow:

  point[x] += light vector * ( z - shadow plane )
  point[y] +=
  point[z] = shadow plane

  1 0 light[x] / light[z]

*/

typedef struct {
	int		i2;
	int		facing;
} edgeDef_t;

#define	MAX_EDGE_DEFS	32

static	edgeDef_t	edgeDefs[SHADER_MAX_VERTEXES][MAX_EDGE_DEFS];
static	int			numEdgeDefs[SHADER_MAX_VERTEXES];
static	int			facing[SHADER_MAX_INDEXES/3];
static vec4_t		extrudedEdges[SHADER_MAX_VERTEXES * 4];
static int			numExtrudedEdges;

void R_AddEdgeDef( int i1, int i2, int facing ) {
	int		c;

	c = numEdgeDefs[ i1 ];
	if ( c == MAX_EDGE_DEFS ) {
		return;		// overflow
	}
	edgeDefs[ i1 ][ c ].i2 = i2;
	edgeDefs[ i1 ][ c ].facing = facing;

	numEdgeDefs[ i1 ]++;
}

static void R_Vk_RenderShadowEdges(VkPipeline vk_pipeline) {

	int i = 0;
	while (i < numExtrudedEdges) {
		int count = numExtrudedEdges - i;
		if (count > (SHADER_MAX_VERTEXES - 1) / 4)
			count = (SHADER_MAX_VERTEXES - 1) / 4;

		Com_Memcpy(tess.xyz, extrudedEdges[i*4], 4 * count * sizeof(vec4_t));
		tess.numVertexes = count * 4;

		for (int k = 0; k < count; k++) {
			tess.indexes[k * 6 + 0] = k * 4 + 0;
			tess.indexes[k * 6 + 1] = k * 4 + 2;
			tess.indexes[k * 6 + 2] = k * 4 + 1;

			tess.indexes[k * 6 + 3] = k * 4 + 2;
			tess.indexes[k * 6 + 4] = k * 4 + 3;
			tess.indexes[k * 6 + 5] = k * 4 + 1;
		}
		tess.numIndexes = count * 6;

		for (int k = 0; k < tess.numVertexes; k++) {
			VectorSet(tess.svars.colors[k], 50, 50, 50);
			tess.svars.colors[k][3] = 255;
		}

		
        vk_bind_geometry();
        vk_shade_geometry(vk_pipeline, false, Vk_Depth_Range::normal);
		

		i += count;
	}
}

void R_RenderShadowEdges( void ) {
/*
	int		i;

#if 0
	int		numTris;

	// dumb way -- render every triangle's edges
	numTris = tess.numIndexes / 3;

	for ( i = 0 ; i < numTris ; i++ ) {
		int		i1, i2, i3;

		if ( !facing[i] ) {
			continue;
		}

		i1 = tess.indexes[ i*3 + 0 ];
		i2 = tess.indexes[ i*3 + 1 ];
		i3 = tess.indexes[ i*3 + 2 ];

		qglBegin( GL_TRIANGLE_STRIP );
		qglVertex3fv( tess.xyz[ i1 ] );
		qglVertex3fv( tess.xyz[ i1 + tess.numVertexes ] );
		qglVertex3fv( tess.xyz[ i2 ] );
		qglVertex3fv( tess.xyz[ i2 + tess.numVertexes ] );
		qglVertex3fv( tess.xyz[ i3 ] );
		qglVertex3fv( tess.xyz[ i3 + tess.numVertexes ] );
		qglVertex3fv( tess.xyz[ i1 ] );
		qglVertex3fv( tess.xyz[ i1 + tess.numVertexes ] );
		qglEnd();
	}
#else
	int		c, c2;
	int		j, k;
	int		i2;
	int		c_edges, c_rejected;
	int		hit[2];

	// an edge is NOT a silhouette edge if its face doesn't face the light,
	// or if it has a reverse paired edge that also faces the light.
	// A well behaved polyhedron would have exactly two faces for each edge,
	// but lots of models have dangling edges or overfanned edges
	c_edges = 0;
	c_rejected = 0;

	for ( i = 0 ; i < tess.numVertexes ; i++ ) {
		c = numEdgeDefs[ i ];
		for ( j = 0 ; j < c ; j++ ) {
			if ( !edgeDefs[ i ][ j ].facing ) {
				continue;
			}

			hit[0] = 0;
			hit[1] = 0;

			i2 = edgeDefs[ i ][ j ].i2;
			c2 = numEdgeDefs[ i2 ];
			for ( k = 0 ; k < c2 ; k++ ) {
				if ( edgeDefs[ i2 ][ k ].i2 == i ) {
					hit[ edgeDefs[ i2 ][ k ].facing ]++;
				}
			}

			// if it doesn't share the edge with another front facing
			// triangle, it is a sil edge
			if ( hit[ 1 ] == 0 ) {
				qglBegin( GL_TRIANGLE_STRIP );
				qglVertex3fv( tess.xyz[ i ] );
				qglVertex3fv( tess.xyz[ i + tess.numVertexes ] );
				qglVertex3fv( tess.xyz[ i2 ] );
				qglVertex3fv( tess.xyz[ i2 + tess.numVertexes ] );
				qglEnd();
				c_edges++;
			} else {
				c_rejected++;
			}
		}
	}
#endif
*/
}

/*
=================
RB_ShadowTessEnd

triangleFromEdge[ v1 ][ v2 ]


  set triangle from edge( v1, v2, tri )
  if ( facing[ triangleFromEdge[ v1 ][ v2 ] ] && !facing[ triangleFromEdge[ v2 ][ v1 ] ) {
  }
=================
*/
void RB_ShadowTessEnd( void ) {
	int		i;
	int		numTris;
	vec3_t	lightDir;

	// we can only do this if we have enough space in the vertex buffers
	if ( tess.numVertexes >= SHADER_MAX_VERTEXES / 2 ) {
		return;
	}

	if ( glConfig.stencilBits < 4 ) {
		return;
	}

	VectorCopy( backEnd.currentEntity->lightDir, lightDir );

	// project vertexes away from light direction
	for ( i = 0 ; i < tess.numVertexes ; i++ ) {
		VectorMA( tess.xyz[i], -512, lightDir, tess.xyz[i+tess.numVertexes] );
	}

	// decide which triangles face the light
	memset( numEdgeDefs, 0, 4 * tess.numVertexes );

	numTris = tess.numIndexes / 3;
	for ( i = 0 ; i < numTris ; i++ ) {
		int		i1, i2, i3;
		vec3_t	d1, d2, normal;
		float	*v1, *v2, *v3;
		float	d;

		i1 = tess.indexes[ i*3 + 0 ];
		i2 = tess.indexes[ i*3 + 1 ];
		i3 = tess.indexes[ i*3 + 2 ];

		v1 = tess.xyz[ i1 ];
		v2 = tess.xyz[ i2 ];
		v3 = tess.xyz[ i3 ];

		VectorSubtract( v2, v1, d1 );
		VectorSubtract( v3, v1, d2 );
		CrossProduct( d1, d2, normal );

		d = DotProduct( normal, lightDir );
		if ( d > 0 ) {
			facing[ i ] = 1;
		} else {
			facing[ i ] = 0;
		}

		// create the edges
		R_AddEdgeDef( i1, i2, facing[ i ] );
		R_AddEdgeDef( i2, i3, facing[ i ] );
		R_AddEdgeDef( i3, i1, facing[ i ] );
	}

	// draw the silhouette edges

	GL_Bind( tr.whiteImage );
	//qglEnable( GL_CULL_FACE );
	GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ZERO );
	/*qglColor3f( 0.2f, 0.2f, 0.2f );

	// don't write to the color buffer
	qglColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

	qglEnable( GL_STENCIL_TEST );
	qglStencilFunc( GL_ALWAYS, 1, 255 );*/

	// mirrors have the culling order reversed
	if ( backEnd.viewParms.isMirror ) {
		/*qglCullFace( GL_FRONT );
		qglStencilOp( GL_KEEP, GL_KEEP, GL_INCR );

		R_RenderShadowEdges();

		qglCullFace( GL_BACK );
		qglStencilOp( GL_KEEP, GL_KEEP, GL_DECR );*/

		R_Vk_RenderShadowEdges(vk.shadow_volume_pipelines[0][1]);
		R_Vk_RenderShadowEdges(vk.shadow_volume_pipelines[1][1]);
	} else {
	/*
		qglCullFace( GL_BACK );
		qglStencilOp( GL_KEEP, GL_KEEP, GL_INCR );

		R_RenderShadowEdges();

		qglCullFace( GL_FRONT );
		qglStencilOp( GL_KEEP, GL_KEEP, GL_DECR );*/

		R_Vk_RenderShadowEdges(vk.shadow_volume_pipelines[0][0]);
		R_Vk_RenderShadowEdges(vk.shadow_volume_pipelines[1][0]);
	}


	// reenable writing to the color buffer
	//qglColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
}


/*
=================
RB_ShadowFinish

Darken everything that is is a shadow volume.
We have to delay this until everything has been shadowed,
because otherwise shadows from different body parts would
overlap and double darken.
=================
*/
void RB_ShadowFinish( void ) {
	if ( r_shadows->integer != 2 ) {
		return;
	}
	if ( glConfig.stencilBits < 4 ) {
		return;
	}
	/*qglEnable( GL_STENCIL_TEST );
	qglStencilFunc( GL_NOTEQUAL, 0, 255 );

	qglDisable (GL_CLIP_PLANE0);
	qglDisable (GL_CULL_FACE);
*/
	GL_Bind( tr.whiteImage );
/*
    qglLoadIdentity ();

	qglColor3f( 0.6f, 0.6f, 0.6f );*/
	GL_State( GLS_DEPTHMASK_TRUE | GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ZERO );
	//Following line makes a kind of flashlight instead of shadows, should multiply though
	//GL_State( GLS_DEPTHMASK_TRUE | GLS_SRCBLEND_DST_COLOR  | GLS_DSTBLEND_ONE );
/*
	qglBegin( GL_QUADS );
	qglVertex3f( -100, 100, -10 );
	qglVertex3f( 100, 100, -10 );
	qglVertex3f( 100, -100, -10 );
	qglVertex3f( -100, -100, -10 );
	qglEnd ();

	qglColor4f(1,1,1,1);
	qglDisable( GL_STENCIL_TEST );*/
	
    tess.indexes[0] = 0;
    tess.indexes[1] = 1;
    tess.indexes[2] = 2;
    tess.indexes[3] = 0;
    tess.indexes[4] = 2;
    tess.indexes[5] = 3;
    tess.numIndexes = 6;

    VectorSet(tess.xyz[0], -100,  100, -10);
    VectorSet(tess.xyz[1],  100,  100, -10);
    VectorSet(tess.xyz[2],  100, -100, -10);
    VectorSet(tess.xyz[3], -100, -100, -10);
    for (int i = 0; i < 4; i++) {
        VectorSet(tess.svars.colors[i], 153, 153, 153);
        tess.svars.colors[i][3] = 255;
    }
    tess.numVertexes = 4;

    // set backEnd.or.modelMatrix to identity matrix
    float tmp[16];
    Com_Memcpy(tmp, vk_world.modelview_transform, 64);
    Com_Memset(vk_world.modelview_transform, 0, 64);
    vk_world.modelview_transform[0] = 1.0f;
    vk_world.modelview_transform[5] = 1.0f;
    vk_world.modelview_transform[10] = 1.0f;
    vk_world.modelview_transform[15] = 1.0f;

    vk_bind_geometry();
    vk_shade_geometry(vk.shadow_finish_pipeline, false, Vk_Depth_Range::normal);

    Com_Memcpy(vk_world.modelview_transform, tmp, 64);

    tess.numIndexes = 0;
    tess.numVertexes = 0;
}


/*
=================
RB_ProjectionShadowDeform

=================
*/
void RB_ProjectionShadowDeform( void ) {
	float	*xyz;
	int		i;
	float	h;
	vec3_t	ground;
	vec3_t	light;
	float	groundDist;
	float	d;
	vec3_t	lightDir;

	xyz = ( float * ) tess.xyz;

	ground[0] = backEnd.ori.axis[0][2];
	ground[1] = backEnd.ori.axis[1][2];
	ground[2] = backEnd.ori.axis[2][2];

	groundDist = backEnd.ori.origin[2] - backEnd.currentEntity->e.shadowPlane;

	VectorCopy( backEnd.currentEntity->lightDir, lightDir );
	d = DotProduct( lightDir, ground );
	// don't let the shadows get too long or go negative
	if ( d < 0.5 ) 
	{
		VectorMA( lightDir, (0.5 - d), ground, lightDir );
		d = DotProduct( lightDir, ground );
	}
	d = 1.0 / d;

	light[0] = lightDir[0] * d;
	light[1] = lightDir[1] * d;
	light[2] = lightDir[2] * d;

	for ( i = 0; i < tess.numVertexes; i++, xyz += 4 ) 
	{
		h = DotProduct( xyz, ground ) + groundDist;

		xyz[0] -= light[0] * h;
		xyz[1] -= light[1] * h;
		xyz[2] -= light[2] * h;
	}
}