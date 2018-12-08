#pragma once
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Camera.h"
#include <math.h>
#include <stack>
#include<windows.h>
#include <mmsystem.h>
#include<dsound.h>
#pragma comment(lib, "WINMM.LIB")

class CubeApp : public D3DApp
{
public:
	CubeApp(HINSTANCE hInstance);
	~CubeApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	//���弸����״
	void BuildShapeGeometryBuffers();
	//����������
	void CheckError(XMFLOAT4X4& m);
private:
	//������״�Ķ��㻺����+����������
	ID3D11Buffer * mShapesVB;
	ID3D11Buffer* mShapesIB;
	
	ID3D11ShaderResourceView* mTexSRV;
	//6�������(�������ڵƹ���������Ȳ���)
	DirectionalLight mDirLights[6];

	//���ϣ��⣩����
	Material mBoxMat;

	//����ӱ��ؿռ䵽����ռ��ת����
	XMFLOAT4X4 mBoxWorld[27];

	int mBoxVertexOffset;

	UINT mBoxIndexOffset;

	UINT mBoxIndexCount;

	Camera mCam;

	bool PressKey;
	bool wasPress;
	bool isDone;
	bool isMess;
	bool PressSpace;

	POINT mLastMousePos;

	XMFLOAT3 m_target;
	XMFLOAT3 m_up;

	enum KEY{
		Q_KEY, W_KEY, E_KEY,
		R_KEY, T_KEY, Y_KEY,
		U_KEY, I_KEY, O_KEY,
		A_KEY, S_KEY, D_KEY,
		F_KEY, G_KEY, H_KEY,
		J_KEY, K_KEY, L_KEY,
		PUP_KEY, PDOWN_KEY,
		UP_KEY, DOWN_KEY,
		LEFT_KEY, RIGHT_KEY,NONE
	} Key;

	std::stack<KEY> s;

	//��¼���Ҵ�����ÿ���һ�μ�һ
	int MessStep;
	//��¼��ת����
	int t;
};