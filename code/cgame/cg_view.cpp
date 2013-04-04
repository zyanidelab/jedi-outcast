// cg_view.c -- setup all the parameters (position, angle, etc)
// for a 3D rendering

// this line must stay at top so the whole PCH thing works...
#include "cg_headers.h"

//#include "cg_local.h"
#include "cg_media.h"
#include "FxScheduler.h"
#include "cg_lights.h"
#include "..\game\wp_saber.h"

float cg_zoomFov;

//#define CG_CAM_ABOVE	2

/*
=============================================================================

  MODEL TESTING

The viewthing and gun positioning tools from Q2 have been integrated and
enhanced into a single model testing facility.

Model viewing can begin with either "testmodel <modelname>" or "testgun <modelname>".

The names must be the full pathname after the basedir, like 
"models/weapons/v_launch/tris.md3" or "players/male/tris.md3"

Testmodel will create a fake entity 100 units in front of the current view
position, directly facing the viewer.  It will remain immobile, so you can
move around it to view it from different angles.

Testgun will cause the model to follow the player around and supress the real
view weapon model.  The default frame 0 of most guns is completely off screen,
so you will probably have to cycle a couple frames to see it.

"nextframe", "prevframe", "nextskin", and "prevskin" commands will change the
frame or skin of the testmodel.  These are bound to F5, F6, F7, and F8 in
q3default.cfg.

If a gun is being tested, the "gun_x", "gun_y", and "gun_z" variables will let
you adjust the positioning.

Note that none of the model testing features update while the game is paused, so
it may be convenient to test with deathmatch set to 1 so that bringing down the
console doesn't pause the game.

=============================================================================
*/

/*
Ghoul2 Insert Start
*/

/*
=================
CG_TestModel_f

Creates an entity in front of the current position, which
can then be moved around
=================
*/
void CG_TestG2Model_f (void) {
	vec3_t		angles;
	CGhoul2Info_v *ghoul2;

	memset( &cg.testModelEntity, 0, sizeof(cg.testModelEntity) );
	ghoul2 = new CGhoul2Info_v;
	cg.testModelEntity.ghoul2 = ghoul2;
	if ( cgi_Argc() < 2 ) {
		return;
	}

	Q_strncpyz (cg.testModelName, CG_Argv( 1 ), MAX_QPATH );
	cg.testModelEntity.hModel = cgi_R_RegisterModel( cg.testModelName );

	cg.testModel = gi.G2API_InitGhoul2Model(*((CGhoul2Info_v *)cg.testModelEntity.ghoul2), cg.testModelName, cg.testModelEntity.hModel, NULL, NULL,0);
	cg.testModelEntity.radius = 100.0f;

	if ( cgi_Argc() == 3 ) {
		cg.testModelEntity.backlerp = atof( CG_Argv( 2 ) );
		cg.testModelEntity.frame = 1;
		cg.testModelEntity.oldframe = 0;
	}
	if (! cg.testModelEntity.hModel ) {
		CG_Printf( "Can't register model\n" );
		return;
	}

	VectorMA( cg.refdef.vieworg, 100, cg.refdef.viewaxis[0], cg.testModelEntity.origin );

	angles[PITCH] = 0;
	angles[YAW] = 180 + cg.refdefViewAngles[1];
	angles[ROLL] = 0;

	AnglesToAxis( angles, cg.testModelEntity.axis );
	cg.testGun = qfalse;
}

void CG_ListModelSurfaces_f (void)
{
	CGhoul2Info_v	&ghoul2 = *((CGhoul2Info_v *)cg.testModelEntity.ghoul2);

 	gi.G2API_ListSurfaces(&ghoul2[cg.testModel]);
}


void CG_ListModelBones_f (void)
{
  	// test to see if we got enough args
	if ( cgi_Argc() < 2 ) 
	{
		return;
	}
	CGhoul2Info_v	&ghoul2 = *((CGhoul2Info_v *)cg.testModelEntity.ghoul2);

	gi.G2API_ListBones(&ghoul2[cg.testModel], atoi(CG_Argv(1)));
}

void CG_TestModelSurfaceOnOff_f(void)
{
	// test to see if we got enough args
	if ( cgi_Argc() < 3 ) 
	{
		return;
	}
	CGhoul2Info_v	&ghoul2 = *((CGhoul2Info_v *)cg.testModelEntity.ghoul2);

	gi.G2API_SetSurfaceOnOff(&ghoul2[cg.testModel], CG_Argv(1), atoi(CG_Argv(2)));
}

void CG_TestModelSetAnglespre_f(void)
{
	vec3_t	angles;

	if ( cgi_Argc() < 3 ) 
	{
		return;
	}
	CGhoul2Info_v	&ghoul2 = *((CGhoul2Info_v *)cg.testModelEntity.ghoul2);

	angles[0] = atof(CG_Argv(2)); 
	angles[1] = atof(CG_Argv(3)); 
	angles[2] = atof(CG_Argv(4));
	gi.G2API_SetBoneAngles(&ghoul2[cg.testModel], CG_Argv(1), angles, BONE_ANGLES_PREMULT, POSITIVE_X, POSITIVE_Z, POSITIVE_Y, NULL);
}

void CG_TestModelSetAnglespost_f(void)
{
	vec3_t	angles;

	if ( cgi_Argc() < 3 ) 
	{
		return;
	}
	CGhoul2Info_v	&ghoul2 = *((CGhoul2Info_v *)cg.testModelEntity.ghoul2);

	angles[0] = atof(CG_Argv(2)); 
	angles[1] = atof(CG_Argv(3)); 
	angles[2] = atof(CG_Argv(4));
	gi.G2API_SetBoneAngles(&ghoul2[cg.testModel], CG_Argv(1), angles, BONE_ANGLES_POSTMULT, POSITIVE_X, POSITIVE_Z, POSITIVE_Y, NULL);
}

void CG_TestModelAnimate_f(void)
{
	char	boneName[100];
	CGhoul2Info_v	&ghoul2 = *((CGhoul2Info_v *)cg.testModelEntity.ghoul2);

	strcpy(boneName, CG_Argv(1));
	gi.G2API_SetBoneAnim(&ghoul2[cg.testModel], boneName, atoi(CG_Argv(2)), atoi(CG_Argv(3)), BONE_ANIM_OVERRIDE_LOOP, atof(CG_Argv(4)), cg.time);

}

/*
Ghoul2 Insert End
*/


/*
=================
CG_TestModel_f

Creates an entity in front of the current position, which
can then be moved around
=================
*/
void CG_TestModel_f (void) {
	vec3_t		angles;

	memset( &cg.testModelEntity, 0, sizeof(cg.testModelEntity) );
	if ( cgi_Argc() < 2 ) {
		return;
	}

	Q_strncpyz (cg.testModelName, CG_Argv( 1 ), MAX_QPATH );
	cg.testModelEntity.hModel = cgi_R_RegisterModel( cg.testModelName );

	if ( cgi_Argc() == 3 ) {
		cg.testModelEntity.backlerp = atof( CG_Argv( 2 ) );
		cg.testModelEntity.frame = 1;
		cg.testModelEntity.oldframe = 0;
	}
	if (! cg.testModelEntity.hModel ) {
		CG_Printf( "Can't register model\n" );
		return;
	}

	VectorMA( cg.refdef.vieworg, 100, cg.refdef.viewaxis[0], cg.testModelEntity.origin );

	angles[PITCH] = 0;
	angles[YAW] = 180 + cg.refdefViewAngles[1];
	angles[ROLL] = 0;

	AnglesToAxis( angles, cg.testModelEntity.axis );
	cg.testGun = qfalse;
}

/*
=================
CG_TestGun_f

Replaces the current view weapon with the given model
=================
*/
void CG_TestGun_f (void) {
	CG_TestModel_f();
	cg.testGun = qtrue;
	cg.testModelEntity.renderfx = RF_DEPTHHACK | RF_FIRST_PERSON;
}


void CG_TestModelNextFrame_f (void) {
	cg.testModelEntity.frame++;
	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelPrevFrame_f (void) {
	cg.testModelEntity.frame--;
	if ( cg.testModelEntity.frame < 0 ) {
		cg.testModelEntity.frame = 0;
	}
	CG_Printf( "frame %i\n", cg.testModelEntity.frame );
}

void CG_TestModelNextSkin_f (void) {
	cg.testModelEntity.skinNum++;
	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

void CG_TestModelPrevSkin_f (void) {
	cg.testModelEntity.skinNum--;
	if ( cg.testModelEntity.skinNum < 0 ) {
		cg.testModelEntity.skinNum = 0;
	}
	CG_Printf( "skin %i\n", cg.testModelEntity.skinNum );
}

static void CG_AddTestModel (void) {
	int		i;

	// re-register the model, because the level may have changed	
/*	cg.testModelEntity.hModel = cgi_R_RegisterModel( cg.testModelName );
	if (! cg.testModelEntity.hModel ) {
		CG_Printf ("Can't register model\n");
		return;
	}
*/
	// if testing a gun, set the origin reletive to the view origin
	if ( cg.testGun ) {
		VectorCopy( cg.refdef.vieworg, cg.testModelEntity.origin );
		VectorCopy( cg.refdef.viewaxis[0], cg.testModelEntity.axis[0] );
		VectorCopy( cg.refdef.viewaxis[1], cg.testModelEntity.axis[1] );
		VectorCopy( cg.refdef.viewaxis[2], cg.testModelEntity.axis[2] );

		// allow the position to be adjusted
		for (i=0 ; i<3 ; i++) {
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[0][i] * cg_gun_x.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[1][i] * cg_gun_y.value;
			cg.testModelEntity.origin[i] += cg.refdef.viewaxis[2][i] * cg_gun_z.value;
		}
	}

	cgi_R_AddRefEntityToScene( &cg.testModelEntity );
}



//============================================================================


/*
=================
CG_CalcVrect

Sets the coordinates of the rendered window
=================
*/
void CG_CalcVrect (void) {
	const int		size = 100;

	cg.refdef.width = cgs.glconfig.vidWidth * size * 0.01;
	cg.refdef.width &= ~1;

	cg.refdef.height = cgs.glconfig.vidHeight * size * 0.01;
	cg.refdef.height &= ~1;

	cg.refdef.x = (cgs.glconfig.vidWidth - cg.refdef.width) * 0.5;
	cg.refdef.y = (cgs.glconfig.vidHeight - cg.refdef.height) * 0.5;
}

//==============================================================================
//==============================================================================
#define CAMERA_DAMP_INTERVAL	50

static vec3_t	cameramins = { -4, -4, -4 };
static vec3_t	cameramaxs = { 4, 4, 4 };
vec3_t	camerafwd, cameraup;

vec3_t	cameraFocusAngles,			cameraFocusLoc;
vec3_t	cameraIdealTarget,			cameraIdealLoc;
vec3_t	cameraCurTarget={0,0,0},	cameraCurLoc={0,0,0};
vec3_t	cameraOldLoc={0,0,0},		cameraNewLoc={0,0,0};
int		cameraLastFrame=0;

/*
===============
Notes on the camera viewpoint in and out...

cg.refdef.vieworg
--at the start of the function holds the player actor's origin (center of player model).
--it is set to the final view location of the camera at the end of the camera code.
cg.refdefViewAngles
--at the start holds the client's view angles
--it is set to the final view angle of the camera at the end of the camera code.

===============
*/
  
/*
===============
CG_CalcTargetThirdPersonViewLocation

===============
*/
static void CG_CalcIdealThirdPersonViewTarget(void)
{
	// Initialize IdealTarget
	VectorCopy(cg.refdef.vieworg, cameraFocusLoc);

	// Add in the new viewheight
	cameraFocusLoc[2] += cg.predicted_player_state.viewheight;

	if ( cg.snap->ps.viewEntity > 0 && cg.snap->ps.viewEntity < ENTITYNUM_WORLD )
	{
		VectorCopy( cameraFocusLoc, cameraIdealTarget );
	}
	else if ( cg.overrides.active & CG_OVERRIDE_3RD_PERSON_VOF )
	{
		// Add in a vertical offset from the viewpoint, which puts the actual target above the head, regardless of angle.
		VectorCopy( cameraFocusLoc, cameraIdealTarget );
		cameraIdealTarget[2] += cg.overrides.thirdPersonVertOffset;
		//VectorMA(cameraFocusLoc, cg.overrides.thirdPersonVertOffset, cameraup, cameraIdealTarget);
	}
	else
	{
		// Add in a vertical offset from the viewpoint, which puts the actual target above the head, regardless of angle.
		VectorCopy( cameraFocusLoc, cameraIdealTarget );
		cameraIdealTarget[2] += cg_thirdPersonVertOffset.value;
		//VectorMA(cameraFocusLoc, cg_thirdPersonVertOffset.value, cameraup, cameraIdealTarget);
	}
}

	

/*
===============
CG_CalcTargetThirdPersonViewLocation

===============
*/
static void CG_CalcIdealThirdPersonViewLocation(void)
{
	if ( cg.overrides.active & CG_OVERRIDE_3RD_PERSON_RNG )
	{
		VectorMA(cameraIdealTarget, -(cg.overrides.thirdPersonRange), camerafwd, cameraIdealLoc);
	}
	else
	{
		VectorMA(cameraIdealTarget, -(cg_thirdPersonRange.value), camerafwd, cameraIdealLoc);
	}
	if ( cg.renderingThirdPerson && (cg.snap->ps.forcePowersActive&(1<<FP_SPEED)) && player->client->ps.forcePowerDuration[FP_SPEED] )
	{
		float timeLeft = player->client->ps.forcePowerDuration[FP_SPEED] - cg.time;
		float length = FORCE_SPEED_DURATION*forceSpeedValue[player->client->ps.forcePowerLevel[FP_SPEED]];
		float amt = forceSpeedRangeMod[player->client->ps.forcePowerLevel[FP_SPEED]];
		if ( timeLeft < 500 )
		{//start going back
			VectorMA(cameraIdealLoc, (timeLeft)/500*amt, camerafwd, cameraIdealLoc);
		}
		else if ( length - timeLeft < 1000 )
		{//start zooming in
			VectorMA(cameraIdealLoc, (length - timeLeft)/1000*amt, camerafwd, cameraIdealLoc);
		}
		else
		{
			VectorMA(cameraIdealLoc, amt, camerafwd, cameraIdealLoc);
		}
	}
}



static void CG_ResetThirdPersonViewDamp(void)
{
	trace_t trace;

	// Cap the pitch within reasonable limits
	if (cameraFocusAngles[PITCH] > 89.0)
	{
		cameraFocusAngles[PITCH] = 89.0;
	}
	else if (cameraFocusAngles[PITCH] < -89.0)
	{
		cameraFocusAngles[PITCH] = -89.0;
	}

	AngleVectors(cameraFocusAngles, camerafwd, NULL, cameraup);

	// Set the cameraIdealTarget
	CG_CalcIdealThirdPersonViewTarget();

	// Set the cameraIdealLoc
	CG_CalcIdealThirdPersonViewLocation();

	// Now, we just set everything to the new positions.
	VectorCopy(cameraIdealLoc, cameraCurLoc);
	VectorCopy(cameraIdealTarget, cameraCurTarget);

	// First thing we do is trace from the first person viewpoint out to the new target location.
	CG_Trace(&trace, cameraFocusLoc, cameramins, cameramaxs, cameraCurTarget, cg.predicted_player_state.clientNum, MASK_SOLID|CONTENTS_PLAYERCLIP);
	if (trace.fraction <= 1.0)
	{
		VectorCopy(trace.endpos, cameraCurTarget);
	}

	// Now we trace from the new target location to the new view location, to make sure there is nothing in the way.
	CG_Trace(&trace, cameraCurTarget, cameramins, cameramaxs, cameraCurLoc, cg.predicted_player_state.clientNum, MASK_SOLID|CONTENTS_PLAYERCLIP);
	if (trace.fraction <= 1.0)
	{
		VectorCopy(trace.endpos, cameraCurLoc);
	}

	cameraLastFrame = cg.time;
}

// This is called every frame.
static void CG_UpdateThirdPersonTargetDamp(void)
{
	trace_t trace;
	vec3_t	targetdiff;
	float	dampfactor, dtime, ratio;

	// Set the cameraIdealTarget
	// Automatically get the ideal target, to avoid jittering.
	CG_CalcIdealThirdPersonViewTarget();

	if (cg_thirdPersonTargetDamp.value>=1.0)//||cg.thisFrameTeleport)
	{	// No damping.
		VectorCopy(cameraIdealTarget, cameraCurTarget);
	}
	else if (cg_thirdPersonTargetDamp.value>=0.0)
	{	
		// Calculate the difference from the current position to the new one.
		VectorSubtract(cameraIdealTarget, cameraCurTarget, targetdiff);

		// Now we calculate how much of the difference we cover in the time allotted.
		// The equation is (Damp)^(time)
		dampfactor = 1.0-cg_thirdPersonTargetDamp.value;	// We must exponent the amount LEFT rather than the amount bled off
		dtime = (float)(cg.time-cameraLastFrame) * (1.0/cg_timescale.value) * (1.0/(float)CAMERA_DAMP_INTERVAL);	// Our dampfactor is geared towards a time interval equal to "1".

		// Note that since there are a finite number of "practical" delta millisecond values possible, 
		// the ratio should be initialized into a chart ultimately.
		ratio = pow(dampfactor, dtime);
		
		// This value is how much distance is "left" from the ideal.
		VectorMA(cameraIdealTarget, -ratio, targetdiff, cameraCurTarget);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	// Now we trace to see if the new location is cool or not.

	// First thing we do is trace from the first person viewpoint out to the new target location.
	CG_Trace(&trace, cameraFocusLoc, cameramins, cameramaxs, cameraCurTarget, cg.predicted_player_state.clientNum, MASK_SOLID|CONTENTS_PLAYERCLIP);
	if (trace.fraction < 1.0)
	{
		VectorCopy(trace.endpos, cameraCurTarget);
	}

	// Note that previously there was an upper limit to the number of physics traces that are done through the world
	// for the sake of camera collision, since it wasn't calced per frame.  Now it is calculated every frame.
	// This has the benefit that the camera is a lot smoother now (before it lerped between tested points),
	// however two full volume traces each frame is a bit scary to think about.
}

// This can be called every interval, at the user's discretion.
static int camWaterAdjust = 0;
static void CG_UpdateThirdPersonCameraDamp(void)
{
	trace_t trace;
	vec3_t	locdiff;
	float dampfactor, dtime, ratio;

	// Set the cameraIdealLoc
	CG_CalcIdealThirdPersonViewLocation();
	
	
	// First thing we do is calculate the appropriate damping factor for the camera.
	dampfactor=0.0;
	if ( cg.overrides.active & CG_OVERRIDE_3RD_PERSON_CDP )
	{
		if ( cg.overrides.thirdPersonCameraDamp != 0.0f )
		{
			double pitch;

			// Note that the camera pitch has already been capped off to 89.
			pitch = Q_fabs(cameraFocusAngles[PITCH]);

			// The higher the pitch, the larger the factor, so as you look up, it damps a lot less.
			pitch /= 89.0;	
			dampfactor = (1.0-cg.overrides.thirdPersonCameraDamp)*(pitch*pitch);

			dampfactor += cg.overrides.thirdPersonCameraDamp;
		}
	}
	else if ( cg_thirdPersonCameraDamp.value != 0.0f )
	{
		double pitch;

		// Note that the camera pitch has already been capped off to 89.
		pitch = Q_fabs(cameraFocusAngles[PITCH]);

		// The higher the pitch, the larger the factor, so as you look up, it damps a lot less.
		pitch /= 89.0;	
		dampfactor = (1.0-cg_thirdPersonCameraDamp.value)*(pitch*pitch);

		dampfactor += cg_thirdPersonCameraDamp.value;
	}

	if (dampfactor>=1.0)//||cg.thisFrameTeleport)
	{	// No damping.
		VectorCopy(cameraIdealLoc, cameraCurLoc);
	}
	else if (dampfactor>=0.0)
	{	
		// Calculate the difference from the current position to the new one.
		VectorSubtract(cameraIdealLoc, cameraCurLoc, locdiff);

		// Now we calculate how much of the difference we cover in the time allotted.
		// The equation is (Damp)^(time)
		dampfactor = 1.0-dampfactor;	// We must exponent the amount LEFT rather than the amount bled off
		dtime = (float)(cg.time-cameraLastFrame) * (1.0/cg_timescale.value) * (1.0/(float)CAMERA_DAMP_INTERVAL);	// Our dampfactor is geared towards a time interval equal to "1".

		// Note that since there are a finite number of "practical" delta millisecond values possible, 
		// the ratio should be initialized into a chart ultimately.
		ratio = pow(dampfactor, dtime);
		
		// This value is how much distance is "left" from the ideal.
		VectorMA(cameraIdealLoc, -ratio, locdiff, cameraCurLoc);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	// Now we trace from the new target location to the new view location, to make sure there is nothing in the way.
	CG_Trace( &trace, cameraCurTarget, cameramins, cameramaxs, cameraCurLoc, cg.predicted_player_state.clientNum, MASK_SOLID|CONTENTS_PLAYERCLIP );
	if ( trace.fraction < 1.0f )
	{
		VectorCopy( trace.endpos, cameraCurLoc );
		//FIXME: when the trace hits movers, it gets very very jaggy... ?
		/*
		//this doesn't actually help any
		if ( trace.entityNum != ENTITYNUM_WORLD )
		{
			centity_t *cent = &cg_entities[trace.entityNum];
			gentity_t *gent = &g_entities[trace.entityNum];
			if ( cent != NULL && gent != NULL )
			{
				if ( cent->currentState.pos.trType == TR_LINEAR || cent->currentState.pos.trType == TR_LINEAR_STOP )
				{
					vec3_t	diff;
					VectorSubtract( cent->lerpOrigin, gent->currentOrigin, diff );
					VectorAdd( cameraCurLoc, diff, cameraCurLoc );
				}
			}
		}
		*/
	}

	// Note that previously there was an upper limit to the number of physics traces that are done through the world
	// for the sake of camera collision, since it wasn't calced per frame.  Now it is calculated every frame.
	// This has the benefit that the camera is a lot smoother now (before it lerped between tested points),
	// however two full volume traces each frame is a bit scary to think about.
}




/*
===============
CG_OffsetThirdPersonView

===============
*/
extern qboolean	MatrixMode;
static void CG_OffsetThirdPersonView( void ) 
{
	vec3_t diff;

	camWaterAdjust = 0;
	// Set camera viewing direction.
	VectorCopy( cg.refdefViewAngles, cameraFocusAngles );

	// if dead, look at killer
	if ( cg.predicted_player_state.stats[STAT_HEALTH] <= 0 ) 
	{
		if ( MatrixMode )
		{
			if ( cg.overrides.active & CG_OVERRIDE_3RD_PERSON_ANG )
			{
				cameraFocusAngles[YAW] += cg.overrides.thirdPersonAngle;
			}
			else
			{
				cameraFocusAngles[YAW] = cg.predicted_player_state.stats[STAT_DEAD_YAW];
				cameraFocusAngles[YAW] += cg_thirdPersonAngle.value;
			}
		}
		else
		{
			cameraFocusAngles[YAW] = cg.predicted_player_state.stats[STAT_DEAD_YAW];
		}
	}
	else
	{	// Add in the third Person Angle.
		if ( cg.overrides.active & CG_OVERRIDE_3RD_PERSON_ANG )
		{
			cameraFocusAngles[YAW] += cg.overrides.thirdPersonAngle;
		}
		else
		{
			cameraFocusAngles[YAW] += cg_thirdPersonAngle.value;
		}
		if ( cg.overrides.active & CG_OVERRIDE_3RD_PERSON_POF )
		{
			cameraFocusAngles[PITCH] += cg.overrides.thirdPersonPitchOffset;
		}
		else
		{
			cameraFocusAngles[PITCH] += cg_thirdPersonPitchOffset.value;
		}
	}

	if ( !cg.renderingThirdPerson && (cg.snap->ps.weapon == WP_SABER||cg.snap->ps.weapon == WP_MELEE) )
	{// First person saber
		// FIXME: use something network-friendly
		vec3_t	org, viewDir;
		VectorCopy( cg_entities[0].gent->client->renderInfo.eyePoint, org );
		float blend = 1.0f - fabs(cg.refdefViewAngles[PITCH])/90.0f;
		AngleVectors( cg.refdefViewAngles, viewDir, NULL, NULL );
		VectorMA( org, -8, viewDir, org );
		VectorScale( org, 1.0f - blend, org );
		VectorMA( org, blend, cg.refdef.vieworg, cg.refdef.vieworg );
		return;
	}
	// The next thing to do is to see if we need to calculate a new camera target location.

	// If we went back in time for some reason, or if we just started, reset the sample.
	if (cameraLastFrame == 0 || cameraLastFrame > cg.time)
	{
		CG_ResetThirdPersonViewDamp();
	}
	else
	{
		// Cap the pitch within reasonable limits
		if (cameraFocusAngles[PITCH] > 89.0)
		{
			cameraFocusAngles[PITCH] = 89.0;
		}
		else if (cameraFocusAngles[PITCH] < -89.0)
		{
			cameraFocusAngles[PITCH] = -89.0;
		}

		AngleVectors(cameraFocusAngles, camerafwd, NULL, cameraup);

		// Move the target to the new location.
		CG_UpdateThirdPersonTargetDamp();
		CG_UpdateThirdPersonCameraDamp();
	}

	// Now interestingly, the Quake method is to calculate a target focus point above the player, and point the camera at it.
	// We won't do that for now.

	// We must now take the angle taken from the camera target and location.
	VectorSubtract(cameraCurTarget, cameraCurLoc, diff);
	float dist = VectorNormalize(diff);
	if ( !dist )
	{//must be hitting something, need some value to calc angles, so use cam forward
		VectorCopy( camerafwd, diff );
	}
	vectoangles(diff, cg.refdefViewAngles);

	// Temp: just move the camera to the side a bit
	extern vmCvar_t cg_thirdPersonHorzOffset;
	if ( cg_thirdPersonHorzOffset.value != 0.0f )
	{
		AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
		VectorMA( cameraCurLoc, cg_thirdPersonHorzOffset.value, cg.refdef.viewaxis[1], cameraCurLoc );
	}

	// ...and of course we should copy the new view location to the proper spot too.
	VectorCopy(cameraCurLoc, cg.refdef.vieworg);

	//if we hit the water, do a last-minute adjustment
	if ( camWaterAdjust )
	{
		cg.refdef.vieworg[2] += camWaterAdjust;
	}
	cameraLastFrame=cg.time;
}



/*
===============
CG_OffsetThirdPersonView

===============
*/
/*
#define	FOCUS_DISTANCE	512
static void CG_OffsetThirdPersonView( void ) {
	vec3_t		forward, right, up;
	vec3_t		view;
	vec3_t		focusAngles;
	trace_t		trace;
	static vec3_t	mins = { -4, -4, -4 };
	static vec3_t	maxs = { 4, 4, 4 };
	vec3_t		focusPoint;
	float		focusDist;
	float		forwardScale, sideScale;

	cg.refdef.vieworg[2] += cg.predicted_player_state.viewheight;

	VectorCopy( cg.refdefViewAngles, focusAngles );

	// if dead, look at killer
	if ( cg.predicted_player_state.stats[STAT_HEALTH] <= 0 ) {
		focusAngles[YAW] = cg.predicted_player_state.stats[STAT_DEAD_YAW];
		cg.refdefViewAngles[YAW] = cg.predicted_player_state.stats[STAT_DEAD_YAW];
	}

	if ( focusAngles[PITCH] > 45 ) {
		focusAngles[PITCH] = 45;		// don't go too far overhead
	}
	AngleVectors( focusAngles, forward, NULL, NULL );

	VectorMA( cg.refdef.vieworg, FOCUS_DISTANCE, forward, focusPoint );

	VectorCopy( cg.refdef.vieworg, view );

	view[2] += 8;

	cg.refdefViewAngles[PITCH] *= 0.5;

	AngleVectors( cg.refdefViewAngles, forward, right, up );

	float tpAngle = cg.overrides.thirdPersonAngle ? cg.overrides.thirdPersonAngle : cg_thirdPersonAngle.value;
	forwardScale = cos( tpAngle / 180 * M_PI );
	sideScale = sin( tpAngle / 180 * M_PI );
	VectorMA( view, -tpAngle * forwardScale, forward, view );
	VectorMA( view, -tpAngle * sideScale, right, view );

	// trace a ray from the origin to the viewpoint to make sure the view isn't
	// in a solid block.  Use an 8 by 8 block to prevent the view from near clipping anything

	CG_Trace( &trace, cg.refdef.vieworg, mins, maxs, view, cg.predicted_player_state.clientNum, MASK_SOLID|CONTENTS_PLAYERCLIP );

	if ( trace.fraction != 1.0 ) {
		VectorCopy( trace.endpos, view );
		view[2] += (1.0 - trace.fraction) * 32;
		// try another trace to this position, because a tunnel may have the ceiling
		// close enogh that this is poking out

		CG_Trace( &trace, cg.refdef.vieworg, mins, maxs, view, cg.predicted_player_state.clientNum, MASK_SOLID|CONTENTS_PLAYERCLIP );
		VectorCopy( trace.endpos, view );
	}


	VectorCopy( view, cg.refdef.vieworg );

	// select pitch to look at focus point from vieword
	VectorSubtract( focusPoint, cg.refdef.vieworg, focusPoint );
	focusDist = sqrt( focusPoint[0] * focusPoint[0] + focusPoint[1] * focusPoint[1] );
	if ( focusDist < 1 ) {
		focusDist = 1;	// should never happen
	}
	cg.refdefViewAngles[PITCH] = -180 / M_PI * atan2( focusPoint[2], focusDist );
	cg.refdefViewAngles[YAW] -= tpAngle;
}


#define MIN_CAMERA_HEIGHT	75
#define MIN_CAMERA_PITCH	40
#define MAX_CAMERA_PITCH	90

//----------------------------------------------
static void CG_OffsetThirdPersonOverheadView( void ) {
	vec3_t		view, angs;
	trace_t		trace;
	static vec3_t	mins = { -4, -4, -4 };
	static vec3_t	maxs = { 4, 4, 4 };

	VectorCopy( cg.refdef.vieworg, view );

	// Move straight up from the player, making sure to always go at least the min camera height, 
	//	otherwise, the camera will clip into the head of the player.
	float tpRange = cg.overrides.thirdPersonRange ? cg.overrides.thirdPersonRange : cg_thirdPersonRange.value;
	if ( tpRange < MIN_CAMERA_HEIGHT )
	{
		view[2] += MIN_CAMERA_HEIGHT;
	}
	else
	{
		view[2] += tpRange;
	}

	// Now adjust the camera angles, but we shouldn't adjust the viewAngles...only the viewAxis
	VectorCopy( cg.refdefViewAngles, angs );
	angs[PITCH] = cg.overrides.thirdPersonAngle ? cg.overrides.thirdPersonAngle : cg_thirdPersonAngle.value;

	// Simple clamp to weed out any really obviously nasty angles
	if ( angs[PITCH] < MIN_CAMERA_PITCH )
	{
		angs[PITCH] = MIN_CAMERA_PITCH;
	}
	else if ( angs[PITCH] > MAX_CAMERA_PITCH )
	{
		angs[PITCH] = MAX_CAMERA_PITCH;
	}

	// Convert our new desired camera angles and store them where they will get used by the engine 
	//	when setting up the actual camera view.
	AnglesToAxis( angs, cg.refdef.viewaxis );
	cg.refdefViewAngles[PITCH] = 0;
	g_entities[0].client->ps.delta_angles[PITCH] = 0;
	
	// Trace a ray from the origin to the viewpoint to make sure the view isn't
	//	in a solid block.
	CG_Trace( &trace, cg.refdef.vieworg, mins, maxs, view, cg.predicted_player_state.clientNum, MASK_SOLID|CONTENTS_PLAYERCLIP );

	if ( trace.fraction != 1.0 ) 
	{
		VectorCopy( trace.endpos, cg.refdef.vieworg );
	}
	else
	{
		VectorCopy( view, cg.refdef.vieworg );
	}
}
*/
// this causes a compiler bug on mac MrC compiler
static void CG_StepOffset( void ) {
	int		timeDelta;
	
	// smooth out stair climbing
	timeDelta = cg.time - cg.stepTime;
	if ( timeDelta < STEP_TIME ) {
		cg.refdef.vieworg[2] -= cg.stepChange 
			* (STEP_TIME - timeDelta) / STEP_TIME;
	}
}

/*
===============
CG_OffsetFirstPersonView

===============
*/
extern qboolean PM_InForceGetUp( playerState_t *ps );
extern qboolean PM_InGetUp( playerState_t *ps );
extern qboolean PM_InKnockDown( playerState_t *ps );
extern int PM_AnimLength( int index, animNumber_t anim );
static void CG_OffsetFirstPersonView( qboolean firstPersonSaber ) {
	float			*origin;
	float			*angles;
	float			bob;
	float			ratio;
	float			delta;
	float			speed;
	float			f;
	vec3_t			predictedVelocity;
	int				timeDelta;
	
	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		return;
	}

	origin = cg.refdef.vieworg;
	angles = cg.refdefViewAngles;

	// if dead, fix the angle and don't add any kick
	if ( cg.snap->ps.stats[STAT_HEALTH] <= 0 ) 
	{
		angles[ROLL] = 40;
		angles[PITCH] = -15;
		angles[YAW] = cg.snap->ps.stats[STAT_DEAD_YAW];
		origin[2] += cg.predicted_player_state.viewheight;
		return;
	}

	if ( g_entities[0].client && PM_InKnockDown( &g_entities[0].client->ps ) )
	{
		float perc, animLen = (float)PM_AnimLength( g_entities[0].client->clientInfo.animFileIndex, (animNumber_t)g_entities[0].client->ps.legsAnim );
		if ( PM_InGetUp( &g_entities[0].client->ps ) || PM_InForceGetUp( &g_entities[0].client->ps ) )
		{//start righting the view
			perc = (float)g_entities[0].client->ps.legsAnimTimer/animLen*2;
		}
		else
		{//tilt the view
			perc = (animLen-g_entities[0].client->ps.legsAnimTimer)/animLen*2;
		}
		if ( perc > 1.0f )
		{
			perc = 1.0f;
		}
		angles[ROLL] = perc*40;
		angles[PITCH] = perc*-15;
	}

	// add angles based on weapon kick
	VectorAdd (angles, cg.kick_angles, angles);

	// add angles based on damage kick
	if ( cg.damageTime ) {
		ratio = cg.time - cg.damageTime;
		if ( ratio < DAMAGE_DEFLECT_TIME ) {
			ratio /= DAMAGE_DEFLECT_TIME;
			angles[PITCH] += ratio * cg.v_dmg_pitch;
			angles[ROLL] += ratio * cg.v_dmg_roll;
		} else {
			ratio = 1.0 - ( ratio - DAMAGE_DEFLECT_TIME ) / DAMAGE_RETURN_TIME;
			if ( ratio > 0 ) {
				angles[PITCH] += ratio * cg.v_dmg_pitch;
				angles[ROLL] += ratio * cg.v_dmg_roll;
			}
		}
	}

	// add pitch based on fall kick
#if 0
	ratio = ( cg.time - cg.landTime) / FALL_TIME;
	if (ratio < 0)
		ratio = 0;
	angles[PITCH] += ratio * cg.fall_value;
#endif

	// add angles based on velocity
	VectorCopy( cg.predicted_player_state.velocity, predictedVelocity );

	delta = DotProduct ( predictedVelocity, cg.refdef.viewaxis[0]);
	angles[PITCH] += delta * cg_runpitch.value;
	
	delta = DotProduct ( predictedVelocity, cg.refdef.viewaxis[1]);
	angles[ROLL] -= delta * cg_runroll.value;

	// add angles based on bob

	// make sure the bob is visible even at low speeds
	speed = cg.xyspeed > 200 ? cg.xyspeed : 200;

	delta = cg.bobfracsin * cg_bobpitch.value * speed;
	if (cg.predicted_player_state.pm_flags & PMF_DUCKED)
		delta *= 3;		// crouching
	angles[PITCH] += delta;
	delta = cg.bobfracsin * cg_bobroll.value * speed;
	if (cg.predicted_player_state.pm_flags & PMF_DUCKED)
		delta *= 3;		// crouching accentuates roll
	if (cg.bobcycle & 1)
		delta = -delta;
	angles[ROLL] += delta;

//===================================

	if ( !firstPersonSaber )//First person saber
	{
		// add view height
		if ( cg.snap->ps.viewEntity > 0 && cg.snap->ps.viewEntity < ENTITYNUM_WORLD )
		{
			if ( &g_entities[cg.snap->ps.viewEntity] &&
				g_entities[cg.snap->ps.viewEntity].client &&
				g_entities[cg.snap->ps.viewEntity].client->ps.viewheight )
			{
				origin[2] += g_entities[cg.snap->ps.viewEntity].client->ps.viewheight;
			}
			else
			{
				origin[2] += 4;//???
			}
		}
		else
		{
			origin[2] += cg.predicted_player_state.viewheight;
		}
	}

	// smooth out duck height changes
	timeDelta = cg.time - cg.duckTime;
	if ( timeDelta < DUCK_TIME) {
		cg.refdef.vieworg[2] -= cg.duckChange * (DUCK_TIME - timeDelta) / DUCK_TIME;
	}

	// add bob height
	bob = cg.bobfracsin * cg.xyspeed * cg_bobup.value;
	if (bob > 6) {
		bob = 6;
	}

	origin[2] += bob;


	// add fall height
	delta = cg.time - cg.landTime;
	if ( delta < LAND_DEFLECT_TIME ) {
		f = delta / LAND_DEFLECT_TIME;
		cg.refdef.vieworg[2] += cg.landChange * f;
	} else if ( delta < LAND_DEFLECT_TIME + LAND_RETURN_TIME ) {
		delta -= LAND_DEFLECT_TIME;
		f = 1.0 - ( delta / LAND_RETURN_TIME );
		cg.refdef.vieworg[2] += cg.landChange * f;
	}

	// add step offset
	CG_StepOffset();

	if(cg.snap->ps.leanofs != 0)
	{
		vec3_t	right;
		//add leaning offset
		//FIXME: when crouching, this bounces up and down?!
		cg.refdefViewAngles[2] += (float)cg.snap->ps.leanofs/2;
		AngleVectors(cg.refdefViewAngles, NULL, right, NULL);
		VectorMA(cg.refdef.vieworg, (float)cg.snap->ps.leanofs, right, cg.refdef.vieworg);
	}

	// pivot the eye based on a neck length
#if 0
	{
#define	NECK_LENGTH		8
	vec3_t			forward, up;
 
	cg.refdef.vieworg[2] -= NECK_LENGTH;
	AngleVectors( cg.refdefViewAngles, forward, NULL, up );
	VectorMA( cg.refdef.vieworg, 3, forward, cg.refdef.vieworg );
	VectorMA( cg.refdef.vieworg, NECK_LENGTH, up, cg.refdef.vieworg );
	}
#endif
}

//======================================================================
/*
void CG_ZoomDown_f( void )
 { 
	// Ignore zoom requests when yer paused
	if ( cg_paused.integer || in_camera )
	{
		return;
	}

	// The zoom hasn't been started yet, so do it now
	if ( cg.zoomMode == 0 )
	{
		cg.zoomLocked = qfalse;
		cg.zoomMode = 1;
		if ( cg.overrides.active & CG_OVERRIDE_FOV )
		{
			cg_zoomFov = cg.overrides.fov;
		}
		else
		{
			cg_zoomFov = cg_fov.value;
		}
		cg.zoomTime = cg.time;
		cgi_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.zoomStart );
		return;
	}

	// Can only snap out of the zoom mode if it has already been locked (CG_ZoomUp_f has been called)
	if ( cg.zoomLocked )
	{
		// Snap out of zoom mode
		cg.zoomMode = 0;
		cg.zoomTime = cg.time;
		cgi_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_AUTO, cgs.media.zoomEnd );
	}
}
 
void CG_ZoomUp_f( void )
 { 
	// Ignore zoom requests when yer paused
	if ( cg_paused.integer || in_camera )
	{
		return;
	}

	if ( cg.zoomMode ) {
		// Freeze the zoom mode
		cg.zoomLocked = qtrue;
	}
}
*/

/*
====================
CG_CalcFovFromX

Calcs Y FOV from given X FOV
====================
*/
#define	WAVE_AMPLITUDE	1
#define	WAVE_FREQUENCY	0.4

qboolean CG_CalcFOVFromX( float fov_x ) 
{
	float	x;
	float	fov_y;
	qboolean	inwater;

	x = cg.refdef.width / tan( fov_x / 360 * M_PI );
	fov_y = atan2( cg.refdef.height, x );
	fov_y = fov_y * 360 / M_PI;

	// there's a problem with this, it only takes the leafbrushes into account, not the entity brushes,
	//	so if you give slime/water etc properties to a func_door area brush in order to move the whole water 
	//	level up/down this doesn't take into account the door position, so warps the view the whole time
	//	whether the water is up or not. Fortunately there's only one slime area in Trek that you can be under,
	//	so lose it...
#if 1
	// warp if underwater
	float	phase;
	float	v;
	int		contents;
	contents = CG_PointContents( cg.refdef.vieworg, -1 );
	if ( contents & ( CONTENTS_WATER | CONTENTS_SLIME | CONTENTS_LAVA ) ){
		phase = cg.time / 1000.0 * WAVE_FREQUENCY * M_PI * 2;
		v = WAVE_AMPLITUDE * sin( phase );
		fov_x += v;
		fov_y -= v;
		inwater = qtrue;
	}
	else {

		inwater = qfalse;
	}
#else
	//inwater = qfalse;
#endif

	// see if we are drugged by an interrogator.  We muck with the FOV here, a bit later, after viewangles are calc'ed, I muck with those too.
	if ( cg.wonkyTime > 0 && cg.wonkyTime > cg.time )
	{
		float perc = (float)(cg.wonkyTime - cg.time) / 10000.0f; // goes for 10 seconds

		fov_x += ( 25.0f * perc );
		fov_y -= ( cos( cg.time * 0.0008f ) * 5.0f * perc );
	}

	// set it
	cg.refdef.fov_x = fov_x;
	cg.refdef.fov_y = fov_y;

	return (inwater);
}

float CG_ForceSpeedFOV( void )
{
	float fov;
	float timeLeft = player->client->ps.forcePowerDuration[FP_SPEED] - cg.time;
	float length = FORCE_SPEED_DURATION*forceSpeedValue[player->client->ps.forcePowerLevel[FP_SPEED]];
	float amt = forceSpeedFOVMod[player->client->ps.forcePowerLevel[FP_SPEED]];
	if ( timeLeft < 500 )
	{//start going back
		fov = cg_fov.value + (timeLeft)/500*amt;
	}
	else if ( length - timeLeft < 1000 )
	{//start zooming in
		fov = cg_fov.value + (length - timeLeft)/1000*amt;
	}
	else
	{//stay at this FOV
		fov = cg_fov.value+amt;
	}
	return fov;
}
/*
====================
CG_CalcFov

Fixed fov at intermissions, otherwise account for fov variable and zooms.
====================
*/
//extern float	g_fov;
static qboolean	CG_CalcFov( void ) {
	float	fov_x;
	float	f;

	if ( cg.predicted_player_state.pm_type == PM_INTERMISSION ) {
		// if in intermission, use a fixed value
		fov_x = 80;
	}
	else if ( cg.snap && cg.snap->ps.viewEntity > 0 && cg.snap->ps.viewEntity < ENTITYNUM_WORLD && !cg.renderingThirdPerson )
	{
		// if in entity camera view, use a special FOV
		if ( &g_entities[cg.snap->ps.viewEntity] &&
			g_entities[cg.snap->ps.viewEntity].NPC )
		{//FIXME: looks bad when take over a jedi... but never really do that, do we?
			fov_x = g_entities[cg.snap->ps.viewEntity].NPC->stats.hfov;
			//sanity-cap?
			if ( fov_x > 120 )
			{
				fov_x = 120;
			}
			else if ( fov_x < 10 )
			{
				fov_x = 10;
			}
		}
		else
		{
			if ( cg.overrides.active & CG_OVERRIDE_FOV )
			{
				fov_x = cg.overrides.fov;
			}
			else
			{
				fov_x = 120;//FIXME: read from the NPC's fov stats?
			}
		}
	} 
	else if ( (cg.snap->ps.forcePowersActive&(1<<FP_SPEED)) && player->client->ps.forcePowerDuration[FP_SPEED] )//cg.renderingThirdPerson && 
	{
		fov_x = CG_ForceSpeedFOV();
	} else {
		// user selectable
		if ( cg.overrides.active & CG_OVERRIDE_FOV )
		{
			fov_x = cg.overrides.fov;
		}
		else
		{
			fov_x = cg_fov.value;
		}
		if ( fov_x < 1 ) {
			fov_x = 1;
		} else if ( fov_x > 160 ) {
			fov_x = 160;
		}

		// Disable zooming when in third person
		if ( cg.zoomMode && cg.zoomMode < 3 )//&& !cg.renderingThirdPerson ) // light amp goggles do none of the zoom silliness
		{
			if ( !cg.zoomLocked )
			{
				if ( cg.zoomMode == 1 )
				{
					// binoculars zooming either in or out
					cg_zoomFov += cg.zoomDir * cg.frametime * 0.05f;
				}
				else
				{
					// disruptor zooming in faster
					cg_zoomFov -= cg.frametime * 0.075f;
				}

				// Clamp zoomFov
				float actualFOV = (cg.overrides.active&CG_OVERRIDE_FOV) ? cg.overrides.fov : cg_fov.value;
				if ( cg_zoomFov < MAX_ZOOM_FOV )
				{
					cg_zoomFov = MAX_ZOOM_FOV;
				}
				else if ( cg_zoomFov > actualFOV )
				{
					cg_zoomFov = actualFOV;
				}
				else
				{//still zooming
					static zoomSoundTime = 0;

					if ( zoomSoundTime < cg.time )
					{
						sfxHandle_t snd;
						
						if ( cg.zoomMode == 1 )
						{
							snd = cgs.media.zoomLoop;
						}
						else
						{
							snd = cgs.media.disruptorZoomLoop;
						}
						
						cgi_S_StartSound( cg.refdef.vieworg, ENTITYNUM_WORLD, CHAN_LOCAL, snd );
						zoomSoundTime = cg.time + 300;
					}
				}
			}

			fov_x = cg_zoomFov;
		} else {
			f = ( cg.time - cg.zoomTime ) / ZOOM_OUT_TIME;
			if ( f > 1.0 ) {
				fov_x = fov_x;
			} else {
				fov_x = cg_zoomFov + f * ( fov_x - cg_zoomFov );
			}
		}
	}

//	g_fov = fov_x;
	return ( CG_CalcFOVFromX( fov_x ) );
}



/*
===============
CG_DamageBlendBlob

===============
*/
static void CG_DamageBlendBlob( void ) 
{
	int			t;
	int			maxTime;
	refEntity_t		ent;

	if ( !cg.damageValue ) {
		return;
	}

	maxTime = DAMAGE_TIME;
	t = cg.time - cg.damageTime;
	if ( t <= 0 || t >= maxTime ) {
		return;
	}

	memset( &ent, 0, sizeof( ent ) );
	ent.reType = RT_SPRITE;
	ent.renderfx = RF_FIRST_PERSON;

	VectorMA( cg.refdef.vieworg, 8, cg.refdef.viewaxis[0], ent.origin );
	VectorMA( ent.origin, cg.damageX * -8, cg.refdef.viewaxis[1], ent.origin );
	VectorMA( ent.origin, cg.damageY * 8, cg.refdef.viewaxis[2], ent.origin );

	ent.radius = cg.damageValue * 3 * ( 1.0 - ((float)t / maxTime) );
	ent.customShader = cgs.media.damageBlendBlobShader;
	ent.shaderRGBA[0] = 180 * ( 1.0 - ((float)t / maxTime) );
	ent.shaderRGBA[1] = 50 * ( 1.0 - ((float)t / maxTime) );
	ent.shaderRGBA[2] = 50 * ( 1.0 - ((float)t / maxTime) );
	ent.shaderRGBA[3] = 255;

	cgi_R_AddRefEntityToScene( &ent );
}

/*
====================
CG_SaberClashFlare
====================
*/
extern int g_saberFlashTime;
extern vec3_t g_saberFlashPos;
extern qboolean CG_WorldCoordToScreenCoord(vec3_t worldCoord, int *x, int *y);
void CG_SaberClashFlare( void ) 
{
	int				t, maxTime = 150;

	t = cg.time - g_saberFlashTime;

	if ( t <= 0 || t >= maxTime ) 
	{
		return;
	}

	vec3_t dif;

	// Don't do clashes for things that are behind us
	VectorSubtract( g_saberFlashPos, cg.refdef.vieworg, dif );

	if ( DotProduct( dif, cg.refdef.viewaxis[0] ) < 0.2 )
	{
		return;
	}

	trace_t tr;

	CG_Trace( &tr, cg.refdef.vieworg, NULL, NULL, g_saberFlashPos, -1, CONTENTS_SOLID );

	if ( tr.fraction < 1.0f )
	{
		return;
	}

	vec3_t color;
	int x,y;
	float v, len = VectorNormalize( dif );

	// clamp to a known range
	if ( len > 800 )
	{
		len = 800;
	}

	v = ( 1.0f - ((float)t / maxTime )) * ((1.0f - ( len / 800.0f )) * 2.0f + 0.35f);

	CG_WorldCoordToScreenCoord( g_saberFlashPos, &x, &y );

	VectorSet( color, 0.8f, 0.8f, 0.8f );
	cgi_R_SetColor( color );

	CG_DrawPic( x - ( v * 300 ), y - ( v * 300 ),
				v * 600, v * 600,
				cgi_R_RegisterShader( "gfx/effects/saberFlare" ));
}

/*
===============
CG_CalcViewValues

Sets cg.refdef view values
===============
*/
static qboolean CG_CalcViewValues( void ) {
	playerState_t	*ps;
	qboolean		viewEntIsCam = qfalse;
	//extern vec3_t	cgRefdefVieworg;

	memset( &cg.refdef, 0, sizeof( cg.refdef ) );

	// calculate size of 3D view
	CG_CalcVrect();

	ps = &cg.predicted_player_state;
#ifndef FINAL_BUILD
	trap_Com_SetOrgAngles(ps->origin,ps->viewangles);
#endif
	// intermission view
	if ( ps->pm_type == PM_INTERMISSION ) {
		VectorCopy( ps->origin, cg.refdef.vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
		AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
		return CG_CalcFov();
	}

	cg.bobcycle = ( ps->bobCycle & 128 ) >> 7;
	cg.bobfracsin = fabs( sin( ( ps->bobCycle & 127 ) / 127.0 * M_PI ) );
	cg.xyspeed = sqrt( ps->velocity[0] * ps->velocity[0] +
		ps->velocity[1] * ps->velocity[1] );


	if ( cg.snap->ps.viewEntity > 0 && cg.snap->ps.viewEntity < ENTITYNUM_WORLD )
	{//in an entity camera view
		if ( g_entities[cg.snap->ps.viewEntity].client && cg.renderingThirdPerson )
		{
			VectorCopy( g_entities[cg.snap->ps.viewEntity].client->renderInfo.eyePoint, cg.refdef.vieworg );
		}
		else
		{
			VectorCopy( cg_entities[cg.snap->ps.viewEntity].lerpOrigin, cg.refdef.vieworg );
		}
		VectorCopy( cg_entities[cg.snap->ps.viewEntity].lerpAngles, cg.refdefViewAngles );
		if ( !Q_stricmp( "misc_camera", g_entities[cg.snap->ps.viewEntity].classname ))
		{
			viewEntIsCam = qtrue;
		}
	}
	else if ( cg.renderingThirdPerson && !cg.zoomMode && (cg.overrides.active&CG_OVERRIDE_3RD_PERSON_ENT) )
	{//different center, same angle
		VectorCopy( cg_entities[cg.overrides.thirdPersonEntity].lerpOrigin, cg.refdef.vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
	}
	else
	{//player's center and angles
		VectorCopy( ps->origin, cg.refdef.vieworg );
		VectorCopy( ps->viewangles, cg.refdefViewAngles );
	}

	// add error decay
	if ( cg_errorDecay.value > 0 ) {
		int		t;
		float	f;

		t = cg.time - cg.predictedErrorTime;
		f = ( cg_errorDecay.value - t ) / cg_errorDecay.value;
		if ( f > 0 && f < 1 ) {
			VectorMA( cg.refdef.vieworg, f, cg.predictedError, cg.refdef.vieworg );
		} else {
			cg.predictedErrorTime = 0;
		}
	}

	if ( (cg.renderingThirdPerson||cg.snap->ps.weapon == WP_SABER||cg.snap->ps.weapon == WP_MELEE) 
		&& !cg.zoomMode 
		&& !viewEntIsCam )
	{
		// back away from character
//		if ( cg_thirdPerson.integer == CG_CAM_ABOVE)
//		{			`
//			CG_OffsetThirdPersonOverheadView();
//		}
//		else
//		{
		// First person saber
		if ( !cg.renderingThirdPerson )
		{
			if ( cg.snap->ps.weapon == WP_SABER||cg.snap->ps.weapon == WP_MELEE )
			{
				vec3_t dir;
				CG_OffsetFirstPersonView( qtrue );
				cg.refdef.vieworg[2] += 32;
				AngleVectors( cg.refdefViewAngles, dir, NULL, NULL );
				VectorMA( cg.refdef.vieworg, -2, dir, cg.refdef.vieworg );
			}
		}
		CG_OffsetThirdPersonView();
//		}
	}  
	else 
	{
		// offset for local bobbing and kicks
		CG_OffsetFirstPersonView( qfalse );
		centity_t	*playerCent = &cg_entities[0];
		if ( playerCent && playerCent->gent && playerCent->gent->client )
		{
			VectorCopy( cg.refdef.vieworg, playerCent->gent->client->renderInfo.eyePoint );
			VectorCopy( cg.refdefViewAngles, playerCent->gent->client->renderInfo.eyeAngles );
			if ( cg.snap->ps.viewEntity > 0 && cg.snap->ps.viewEntity < ENTITYNUM_WORLD )
			{//in an entity camera view
				if ( cg_entities[cg.snap->ps.viewEntity].gent->client )
				{//looking through a client's eyes
					VectorCopy( cg.refdef.vieworg, cg_entities[cg.snap->ps.viewEntity].gent->client->renderInfo.eyePoint );
					VectorCopy( cg.refdefViewAngles, cg_entities[cg.snap->ps.viewEntity].gent->client->renderInfo.eyeAngles );
				}
				else
				{//looking through a regular ent's eyes
					VectorCopy( cg.refdef.vieworg, cg_entities[cg.snap->ps.viewEntity].lerpOrigin );
					VectorCopy( cg.refdefViewAngles, cg_entities[cg.snap->ps.viewEntity].lerpAngles );
				}
			}
			VectorCopy( playerCent->gent->client->renderInfo.eyePoint, playerCent->gent->client->renderInfo.headPoint );
			if ( cg.snap->ps.viewEntity <= 0 || cg.snap->ps.viewEntity >= ENTITYNUM_WORLD )
			{//not in entity cam
				playerCent->gent->client->renderInfo.headPoint[2] -= 8;
			}
		}
	}

	//VectorCopy( cg.refdef.vieworg, cgRefdefVieworg );
	// shake the camera if necessary
	CGCam_UpdateSmooth( cg.refdef.vieworg, cg.refdefViewAngles );
	CGCam_UpdateShake( cg.refdef.vieworg, cg.refdefViewAngles );

	// see if we are drugged by an interrogator.  We muck with the angles here, just a bit earlier, we mucked with the FOV
	if ( cg.wonkyTime > 0 && cg.wonkyTime > cg.time )
	{
		float perc = (float)(cg.wonkyTime - cg.time) / 10000.0f; // goes for 10 seconds

		cg.refdefViewAngles[ROLL] += ( sin( cg.time * 0.0004f )  * 7.0f * perc );
		cg.refdefViewAngles[PITCH] += ( 26.0f * perc + sin( cg.time * 0.0011f ) * 3.0f * perc );
	}

	AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );

	if ( cg.hyperspace ) 
	{
		cg.refdef.rdflags |= RDF_NOWORLDMODEL | RDF_HYPERSPACE;
	}

	// field of view
	return CG_CalcFov();
}


/*
=====================
CG_PowerupTimerSounds
=====================
*/
static void CG_PowerupTimerSounds( void ) 
{
	int	i, time;
	
	// powerup timers going away
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) 
	{
		time = cg.snap->ps.powerups[i];

		if ( time > 0 && time < cg.time ) 
		{
			// add any special powerup expiration sounds here
//			switch( i )
//			{
//			case PW_WEAPON_OVERCHARGE:
//				cgi_S_StartSound( NULL, cg.snap->ps.clientNum, CHAN_ITEM, cgs.media.overchargeEndSound );
//				break;
//			}
		}
	}
}

//----------------------------
void CG_RunEmplacedWeapon()
{
	gentity_t	*player = &g_entities[0],
				*gun = player->owner;

	// Override the camera when we are locked onto the gun.
	if ( player && gun && ( player->s.eFlags & EF_LOCKED_TO_WEAPON ))
	{
//		float dist = -1; // default distance behind gun

		// don't let the player try and change this
		cg.renderingThirdPerson = qtrue;

//		cg.refdefViewAngles[PITCH] += cg.overrides.thirdPersonPitchOffset? cg.overrides.thirdPersonPitchOffset: cg_thirdPersonPitchOffset.value;
//		cg.refdefViewAngles[YAW] += cg.overrides.thirdPersonAngle ? cg.overrides.thirdPersonAngle : cg_thirdPersonAngle.value;;

		AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );

		// Slide in behind the gun.
//		if ( gun->delay + 500 > cg.time )
//		{
//			dist -= (( gun->delay + 500 ) - cg.time ) * 0.02f;
//		}

		VectorCopy( gun->pos2, cg.refdef.vieworg );
		VectorMA( cg.refdef.vieworg, -20.0f, gun->pos3, cg.refdef.vieworg );
		if ( cg.snap->ps.viewEntity <= 0 || cg.snap->ps.viewEntity >= ENTITYNUM_WORLD )
		{
			VectorMA( cg.refdef.vieworg, 35.0f, gun->pos4, cg.refdef.vieworg );
		}

	}
}

//=========================================================================

/*
=================
CG_DrawActiveFrame

Generates and draws a game scene and status information at the given time.
=================
*/
extern void CG_BuildSolidList( void );
void CG_DrawActiveFrame( int serverTime, stereoFrame_t stereoView ) {
	qboolean	inwater = qfalse;

	cg.time = serverTime;

	// update cvars
	CG_UpdateCvars();

	// if we are only updating the screen as a loading
	// pacifier, don't even try to read snapshots
	if ( cg.infoScreenText[0] != 0 ) {
		CG_DrawInformation();
		return;
	}

	CG_RunLightStyles();

	// any looped sounds will be respecified as entities
	// are added to the render list
	cgi_S_ClearLoopingSounds();

	// clear all the render lists
	cgi_R_ClearScene();

	// set up cg.snap and possibly cg.nextSnap
	CG_ProcessSnapshots();

	CG_BuildSolidList();

	// if we haven't received any snapshots yet, all
	// we can draw is the information screen
	if ( !cg.snap ) {
		CG_DrawInformation();
		return;
	}

	// make sure the lagometerSample and frame timing isn't done twice when in stereo
	if ( stereoView != STEREO_RIGHT ) {
		cg.frametime = cg.time - cg.oldTime;
		cg.oldTime = cg.time;
	}
	// Make sure the helper has the updated time
	theFxHelper.AdjustTime( cg.frametime );

	// let the client system know what our weapon and zoom settings are
	//FIXME: should really send forcePowersActive over network onto cg.snap->ps...
	float speed = cg.refdef.fov_y / 75.0 * ((cg_entities[0].gent->client->ps.forcePowersActive&(1<<FP_SPEED))?1.0f:cg_timescale.value);

//FIXME: junk code, BUG:168

static bool wasForceSpeed=false;
bool isForceSpeed=cg_entities[0].gent->client->ps.forcePowersActive&(1<<FP_SPEED)?true:false;
if (isForceSpeed&&!wasForceSpeed)
{
	CGCam_Smooth(0.75f,5000);
}
wasForceSpeed=isForceSpeed;

//

	if ( cg_entities[0].gent->s.eFlags & EF_LOCKED_TO_WEAPON && 
				cg.snap->ps.clientNum == 0 ) 
	{
		speed *= 0.25f;
	}
	cgi_SetUserCmdValue( cg.weaponSelect, speed );

	// this counter will be bumped for every valid scene we generate
	cg.clientFrame++;

	// update cg.predicted_player_state
	CG_PredictPlayerState();

	// decide on third person view
	cg.renderingThirdPerson = cg_thirdPerson.integer || (cg.snap->ps.stats[STAT_HEALTH] <= 0) || (g_entities[0].client&&g_entities[0].client->NPC_class==CLASS_ATST);

	if ( cg.zoomMode )
	{
		// zoomed characters should never do third person stuff??
		cg.renderingThirdPerson = qfalse;
	}

	if ( in_camera )
	{
		// The camera takes over the view
		CGCam_RenderScene();		
	}
	else
	{		
		//Finish any fading that was happening
		CGCam_UpdateFade();
		// build cg.refdef
		inwater = CG_CalcViewValues();
	}

	//check for opaque water
	if ( 1 )
	{
		vec3_t	camTest;
		VectorCopy( cg.refdef.vieworg, camTest );
		camTest[2] += 6;
		if ( !(CG_PointContents( camTest, ENTITYNUM_NONE )&CONTENTS_SOLID) && !gi.inPVS( cg.refdef.vieworg, camTest ) )
		{//crossed visible line into another room
			cg.refdef.vieworg[2] -= 6;
		}
		else
		{
			VectorCopy( cg.refdef.vieworg, camTest );
			camTest[2] -= 6;
			if ( !(CG_PointContents( camTest, ENTITYNUM_NONE )&CONTENTS_SOLID) && !gi.inPVS( cg.refdef.vieworg, camTest ) )
			{
				cg.refdef.vieworg[2] += 6;
			}
		}
		/*
		if ( (trace.contents&(CONTENTS_WATER|CONTENTS_OPAQUE)) )
		{//opaque water
		}
		*/
	}
	//This is done from the vieworg to get origin for non-attenuated sounds
	cgi_S_UpdateAmbientSet( CG_ConfigString( CS_AMBIENT_SET ), cg.refdef.vieworg );

	// first person blend blobs, done after AnglesToAxis
	if ( !cg.renderingThirdPerson ) {
		CG_DamageBlendBlob();		
	}

	// build the render lists
	if ( !cg.hyperspace ) {
		CG_AddPacketEntities();			// adter calcViewValues, so predicted player state is correct
		CG_AddMarks();
		CG_AddLocalEntities();
	}

	// NOTE: this may completely override the camera
	CG_RunEmplacedWeapon();

	// Don't draw the in-view weapon when in camera mode
	if ( !in_camera 
		&& !cg_pano.integer 
		&& cg.snap->ps.weapon != WP_SABER
		&& ( cg.snap->ps.viewEntity == 0 || cg.snap->ps.viewEntity >= ENTITYNUM_WORLD ) )
	{
		CG_AddViewWeapon( &cg.predicted_player_state );
	}

	if ( !cg.hyperspace ) 
	{
		//Add all effects
		theFxScheduler.AddScheduledEffects( );
	}

	// finish up the rest of the refdef
	if ( cg.testModelEntity.hModel ) {
		CG_AddTestModel();
	}
	
	cg.refdef.time = cg.time;
	memcpy( cg.refdef.areamask, cg.snap->areamask, sizeof( cg.refdef.areamask ) );

	// update audio positions
	//NOTE: if we want to make you be able to hear far away sounds with electrobinoculars, add the hacked-in positional offset here (base on fov)
	cgi_S_Respatialize( cg.snap->ps.clientNum, cg.refdef.vieworg, cg.refdef.viewaxis, inwater );

	// warning sounds when powerup is wearing off
	CG_PowerupTimerSounds();

	if ( cg_pano.integer ) {	// let's grab a panorama!
		cg.levelShot = qtrue;  //hide the 2d
		VectorClear(cg.refdefViewAngles);		
		cg.refdefViewAngles[YAW] = -360 * cg_pano.integer/cg_panoNumShots.integer;	//choose angle
		AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
		CG_DrawActive( stereoView );
		cg.levelShot = qfalse;
	} 	else {
		// actually issue the rendering calls
		CG_DrawActive( stereoView );
	}
}
