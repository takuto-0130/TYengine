#include "ModelLoader.h"

namespace TYEngine {
namespace Graphics {

using namespace Core; // For DirectXBasis

void ModelLoader::Initialize(DirectXBasis* dxBasis, SrvManager* srvManager)
{
	dxBasis_ = dxBasis;
	srvManager_ = srvManager;
}

} // namespace Graphics
} // namespace TYEngine
