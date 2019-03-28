#pragma once
#ifndef TRACKBALL_H
#define TRACKBALL_H


//Camera var
double prevMouseX, prevMouseY;
bool mouseLeftPressed;
bool mouseMiddlePressed;
bool mouseRightPressed;
float curr_quat[4];
float prev_quat[4];
float eye[3], lookat[3], up[3];

/*
 * result rotation from last x and y mouse positions
 * Result on first param
 */
void trackball(float q[4], float p1x, float p1y, float p2x, float p2y);

void negate_quat(float *q, float *qn);

/*
 * add of two quaternions
 */
void add_quats(float *q1, float *q2, float *dest);

/*
 * Build a rotation matrix in Matrix based on
 * given quaternion.
 */
void build_rotmatrix(float m[4][4], const float q[4]);

/*
 * Provides a quaternion based on an axis (defined by
 * the given vector) and an angle about which to rotate.  
 * Result on third param
 */
void axis_to_quat(float a[3], float phi, float q[4]);


#endif // TRACKBALL_H