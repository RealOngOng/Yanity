#pragma once

class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRoattion(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRoattion();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMMATRIX m_viewMatrix;
};