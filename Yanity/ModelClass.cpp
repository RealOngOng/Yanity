#include "stdafx.h"
#include "ModelClass.h"

ModelClass::ModelClass()
{

}
ModelClass::ModelClass(const ModelClass& other)
{

}
ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device)
{
	return InitializeBuffers(device);
}
void ModelClass::Shutdown()
{
	ShutdownBuffers();
}
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	m_vertexCount = 5;
	m_indexCount = 9;

	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices) return false;

	ULONG* indices = new ULONG[m_indexCount];
	if (!indices) return false;

	vertices[0].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);

	vertices[1].position = XMFLOAT3(1.0f, 0.5f, 0.0f);
	vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);

	vertices[2].position = XMFLOAT3(0.7f, -1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);

	vertices[3].position = XMFLOAT3(-0.7f, -1.0f, 0.0f);
	vertices[3].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);

	vertices[4].position = XMFLOAT3(-1.0f, 0.5f, 0.0f);
	vertices[4].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;
	indices[6] = 0;
	indices[7] = 3;
	indices[8] = 4;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
		return false;

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
		return false;

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}
void ModelClass::ShutdownBuffers()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
}
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}