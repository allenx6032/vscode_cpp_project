#include <blah_graphics.h>
#include <blah_stream.h>
#include "blah_internal.h"
#include <string.h> // for strcmp

using namespace Blah;

const BlendMode BlendMode::Normal = BlendMode(
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::OneMinusSrcAlpha,
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::OneMinusSrcAlpha,
	BlendMask::RGBA,
	0xffffffff
);

const BlendMode BlendMode::NonPremultiplied = BlendMode(
	BlendOp::Add,
	BlendFactor::SrcAlpha,
	BlendFactor::OneMinusSrcAlpha,
	BlendOp::Add,
	BlendFactor::SrcAlpha,
	BlendFactor::OneMinusSrcAlpha,
	BlendMask::RGBA,
	0xffffffff
);

const BlendMode BlendMode::Subtract = BlendMode(
	BlendOp::ReverseSubtract,
	BlendFactor::One,
	BlendFactor::One,
	BlendOp::Add,
	BlendFactor::One,
	BlendFactor::One,
	BlendMask::RGBA,
	0xffffffff
);

const BlendMode BlendMode::Additive = BlendMode(
	BlendOp::Add,
	BlendFactor::SrcAlpha,
	BlendFactor::One,
	BlendOp::Add,
	BlendFactor::SrcAlpha,
	BlendFactor::One,
	BlendMask::RGBA,
	0xffffffff
);


VertexFormat::VertexFormat(const StackVector<VertexAttribute, 16>& attr, int str)
{
	attributes = attr;
	stride = str;

	if (stride <= 0)
	{
		stride = 0;

		for (auto& it : attributes)
		{
			switch (it.type)
			{
			case VertexType::None: break;
			case VertexType::Float: stride += 4; break;
			case VertexType::Float2: stride += 8; break;
			case VertexType::Float3: stride += 12; break;
			case VertexType::Float4: stride += 16; break;
			case VertexType::Byte4: stride += 4; break;
			case VertexType::UByte4: stride += 4; break;
			case VertexType::Short2: stride += 4; break;
			case VertexType::UShort2: stride += 4; break;
			case VertexType::Short4: stride += 8; break;
			case VertexType::UShort4: stride += 8; break;
			}
		}
	}
}

ShaderRef Shader::create(const ShaderData& data)
{
	BLAH_ASSERT_RENDERER();
	BLAH_ASSERT(data.vertex.length() > 0, "Must provide a Vertex Shader");
	BLAH_ASSERT(data.fragment.length() > 0, "Must provide a Fragment Shader");
	BLAH_ASSERT(data.hlsl_attributes.size() > 0 || App::renderer().type != RendererType::D3D11, "D3D11 Shaders must have hlsl_attributes assigned");

	ShaderRef shader;

	if (auto renderer = Internal::app_renderer())
		shader = renderer->create_shader(&data);

	// validate the shader
	if (shader)
	{
		auto& uniforms = shader->uniforms();

		// make sure its uniforms are valid
		for (auto& it : uniforms)
			if (it.type == UniformType::None)
			{
				auto error = String::fmt("Uniform '%s' has an invalid type!\n\tOnly Float/Float2/Float3/Float4/Mat3x2/Mat4x4/Texture are allowed!", it.name.cstr());
				BLAH_ASSERT(false, error.cstr());
				return ShaderRef();
			}

		// make sure uniform names don't overlap
		for (int i = 0; i < uniforms.size(); i++)
			for (int j = i + 1; j < uniforms.size(); j++)
				if (uniforms[i].name == uniforms[j].name)
				{
					auto error = String::fmt("Shader Uniform names '%s' overlap! All Names must be unique.", uniforms[0].name.cstr());
					BLAH_ASSERT(false, error.cstr());
					return ShaderRef();
				}
	}

	return shader;
}

TextureRef Texture::create(const Image& image)
{
	return create(image.width, image.height, TextureFormat::RGBA, (unsigned char*)image.pixels);
}

TextureRef Texture::create(int width, int height, TextureFormat format, unsigned char* data)
{
	BLAH_ASSERT_RENDERER();
	BLAH_ASSERT(width > 0 && height > 0, "Texture width and height must be larger than 0");
	BLAH_ASSERT((int)format > (int)TextureFormat::None && (int)format < (int)TextureFormat::Count, "Invalid texture format");

	if (auto renderer = Internal::app_renderer())
	{
		auto tex = renderer->create_texture(width, height, format);

		if (tex && data != nullptr)
			tex->set_data(data);

		return tex;
	}

	return TextureRef();
}

TextureRef Texture::create(Stream& stream)
{
	return create(Image(stream));
}

TextureRef Texture::create(const FilePath& file)
{
	return create(Image(file));
}

void Texture::set_data(const Color* data)
{
	if (format() == TextureFormat::RGBA)
		set_data((u8*)data);
}

void Texture::get_data(Color* data)
{
	if (format() == TextureFormat::RGBA)
		get_data((u8*)data);
}

TargetRef Target::create(int width, int height)
{
	AttachmentFormats formats;
	formats.push_back(TextureFormat::RGBA);
	return create(width, height, formats);
}

TargetRef Target::create(int width, int height, const AttachmentFormats& textures)
{
	BLAH_ASSERT_RENDERER();
	BLAH_ASSERT(width > 0 && height > 0, "Target width and height must be larger than 0");
	BLAH_ASSERT(textures.size() > 0, "At least one texture must be provided");

	int color_count = 0;
	int depth_count = 0;

	for (int i = 0; i < textures.size(); i++)
	{
		BLAH_ASSERT((int)textures[i] > (int)TextureFormat::None && (int)textures[i] < (int)TextureFormat::Count, "Invalid texture format");

		if (textures[i] == TextureFormat::DepthStencil)
			depth_count++;
		else
			color_count++;
	}

	BLAH_ASSERT(depth_count <= 1, "Target can only have 1 Depth/Stencil Texture");
	BLAH_ASSERT(color_count <= Attachments::capacity - 1, "Exceeded maximum Color texture count");

	if (auto renderer = Internal::app_renderer())
		return renderer->create_target(width, height, textures.data(), textures.size());

	return TargetRef();
}

TextureRef& Target::texture(int index)
{
	return textures()[index];
}

const TextureRef& Target::texture(int index) const
{
	return textures()[index];
}

int Target::width() const
{
	return textures()[0]->width();
}

int Target::height() const
{
	return textures()[0]->height();
}

MeshRef Mesh::create()
{
	BLAH_ASSERT_RENDERER();

	if (auto renderer = Internal::app_renderer())
		return renderer->create_mesh();

	return MeshRef();
}

namespace
{
	int blah_calc_uniform_size(const UniformInfo& uniform)
	{
		int components = 0;

		switch (uniform.type)
		{
		case UniformType::Float: components = 1; break;
		case UniformType::Float2: components = 2; break;
		case UniformType::Float3: components = 3; break;
		case UniformType::Float4: components = 4; break;
		case UniformType::Mat3x2: components = 6; break;
		case UniformType::Mat4x4: components = 16; break;
		default:
			BLAH_ASSERT(false, "Unespected Uniform Type");
			break;
		}

		return components * uniform.array_length;
	}
}

Material::Material(const ShaderRef& shader)
{
	BLAH_ASSERT(shader, "Material is being created with an invalid shader");
	m_shader = shader;

	auto& uniforms = shader->uniforms();
	int float_size = 0;

	for (auto& uniform : uniforms)
	{
		if (uniform.type == UniformType::None)
			continue;

		if (uniform.type == UniformType::Texture2D)
		{
			if (m_textures.size() < uniform.register_index + uniform.array_length)
				m_textures.resize(uniform.register_index + uniform.array_length);
			continue;
		}

		if (uniform.type == UniformType::Sampler2D)
		{
			if (m_samplers.size() < uniform.register_index + uniform.array_length)
				m_samplers.resize(uniform.register_index + uniform.array_length);
			continue;
		}

		float_size += blah_calc_uniform_size(uniform);
	}

	m_data.expand(float_size);
}

MaterialRef Material::create(const ShaderRef& shader)
{
	BLAH_ASSERT(shader, "The provided shader is invalid");

	if (shader)
		return MaterialRef(new Material(shader));

	return MaterialRef();
}

MaterialRef Material::clone() const
{
	auto copy = MaterialRef(new Material(m_shader));
	copy->m_textures = m_textures;
	copy->m_samplers = m_samplers;
	copy->m_data = m_data;
	return copy;
}

ShaderRef Material::shader() const
{
	return m_shader;
}

void Material::set_texture(const char* name, const TextureRef& texture, int index)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Texture2D)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			if (uniform.register_index + index < m_textures.size())
			{
				m_textures[uniform.register_index + index] = texture;
				return;
			}
			break;
		}
	}

	Log::warn("No Texture Uniform '%s' at index [%i] exists", name, index);
}

void Material::set_texture(int register_index, const TextureRef& texture)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	if (register_index >= m_textures.size())
	{
		Log::warn("Texture Register index [%i] is out of bounds", register_index);
		return;
	}

	m_textures[register_index] = texture;
}

TextureRef Material::get_texture(const char* name, int index) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Texture2D)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			if (uniform.register_index + index < m_textures.size())
				return m_textures[uniform.register_index + index];
			break;
		}
	}

	Log::warn("No Texture Uniform '%s' at index [%i] exists", name, index);
	return TextureRef();
}

TextureRef Material::get_texture(int register_index) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	if (register_index < m_textures.size())
		return m_textures[register_index];

	Log::warn("No Texture Uniform at register index [%i]", register_index);
	return TextureRef();
}

void Material::set_sampler(const char* name, const TextureSampler& sampler, int index)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Sampler2D)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			if (uniform.register_index + index < m_samplers.size())
			{
				m_samplers[uniform.register_index + index] = sampler;
				return;
			}
			break;
		}
	}

	Log::warn("No Texture Sampler Uniform '%s' at index [%i] exists", name, index);
}

void Material::set_sampler(int register_index, const TextureSampler& sampler)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	if (register_index >= m_samplers.size())
	{
		Log::warn("Texture Sampler Register index [%i] is out of bounds", register_index);
		return;
	}

	m_samplers[register_index] = sampler;
}

TextureSampler Material::get_sampler(const char* name, int index) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type != UniformType::Sampler2D)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			if (uniform.register_index + index < m_samplers.size())
				return m_samplers[uniform.register_index + index];
			break;
		}
	}

	Log::warn("No Texture Sampler Uniform '%s' at index [%i] exists", name, index);
	return TextureSampler();
}

TextureSampler Material::get_sampler(int register_index) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	if (register_index < m_samplers.size())
		return m_samplers[register_index];

	Log::warn("No Texture Sampler Uniform at register index [%i]", register_index);
	return TextureSampler();
}

void Material::set_value(const char* name, const float* value, i64 length)
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");
	BLAH_ASSERT(length >= 0, "Length must be >= 0");

	int index = 0;
	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture2D ||
			uniform.type == UniformType::Sampler2D ||
			uniform.type == UniformType::None)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			auto max = blah_calc_uniform_size(uniform);
			if (length > max)
			{
				Log::warn("Exceeding length of Uniform '%s' (%i / %i)", name, length, max);
				length = max;
			}

			memcpy(m_data.begin() + offset, value, sizeof(float) * length);
			return;
		}

		offset += blah_calc_uniform_size(uniform);
		index++;
	}

	Log::warn("No Uniform '%s' exists", name);
}

void Material::set_value(const char* name, float value)
{
	set_value(name, &value, 1);
}

void Material::set_value(const char* name, const Vec2f& value)
{
	set_value(name, &value.x, 2);
}

void Material::set_value(const char* name, const Vec3f& value)
{
	set_value(name, &value.x, 3);
}

void Material::set_value(const char* name, const Vec4f& value)
{
	set_value(name, &value.x, 4);
}

void Material::set_value(const char* name, const Mat3x2f& value)
{
	set_value(name, &value.m11, 6);
}

void Material::set_value(const char* name, const Mat4x4f& value)
{
	set_value(name, &value.m11, 16);
}

void Material::set_value(const char* name, const Vector<float>& value)
{
	set_value(name, value.data(), value.size());
}

void Material::set_value(const char* name, const Vector<Vec2f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 2);
}

void Material::set_value(const char* name, const Vector<Vec3f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 3);
}

void Material::set_value(const char* name, const Vector<Vec4f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 4);
}

void Material::set_value(const char* name, const Vector<Mat3x2f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 6);
}

void Material::set_value(const char* name, const Vector<Mat4x4f>& value)
{
	set_value(name, (float*)value.data(), value.size() * 16);
}

const float* Material::get_value(const char* name, i64* length) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	int index = 0;
	int offset = 0;
	for (auto& uniform : m_shader->uniforms())
	{
		if (uniform.type == UniformType::Texture2D ||
			uniform.type == UniformType::Sampler2D ||
			uniform.type == UniformType::None)
			continue;

		if (strcmp(uniform.name, name) == 0)
		{
			if (length != nullptr)
				*length = blah_calc_uniform_size(uniform);
			return m_data.begin() + offset;
		}

		index++;
		offset += blah_calc_uniform_size(uniform);
	}

	Log::warn("No Uniform '%s' exists", name);
	*length = 0;
	return nullptr;
}

bool Material::has_value(const char* name) const
{
	BLAH_ASSERT(m_shader, "Material Shader is invalid");

	if (name != nullptr && name[0] != '\0')
	{
		for (auto& uniform : m_shader->uniforms())
			if (strcmp(uniform.name, name) == 0)
				return true;
	}

	return false;
}

const Vector<TextureRef>& Material::textures() const
{
	return m_textures;
}

const Vector<TextureSampler>& Material::samplers() const
{
	return m_samplers;
}

const float* Material::data() const
{
	return m_data.begin();
}


DrawCall::DrawCall()
{
	blend = BlendMode::Normal;
	target = App::backbuffer();
	mesh = MeshRef();
	material = MaterialRef();
	has_viewport = false;
	has_scissor = false;
	viewport = Rectf();
	scissor = Rectf();
	index_start = 0;
	index_count = 0;
	instance_count = 0;
	depth = Compare::None;
	cull = Cull::None;
}

void DrawCall::perform()
{
	BLAH_ASSERT_RENDERER();
	BLAH_ASSERT(material, "Trying to draw with an invalid Material");
	BLAH_ASSERT(material->shader(), "Trying to draw with an invalid Shader");
	BLAH_ASSERT(mesh, "Trying to draw with an invalid Mesh");

	if (!Internal::app_renderer())
		return;

	// copy call
	DrawCall pass = *this;

	// Validate Backbuffer
	if (!pass.target)
	{
		pass.target = App::backbuffer();
		Log::warn("Trying to draw with an invalid Target; falling back to Back Buffer");
	}

	// Validate Index Count
	i64 index_count = pass.mesh->index_count();
	if (pass.index_start + pass.index_count > index_count)
	{
		Log::warn(
			"Trying to draw more indices than exist in the index buffer (%i-%i / %i); trimming extra indices",
			pass.index_start,
			pass.index_start + pass.index_count,
			index_count);

		if (pass.index_start > pass.index_count)
			return;

		pass.index_count = pass.index_count - pass.index_start;
	}

	// Validate Instance Count
	i64 instance_count = pass.mesh->instance_count();
	if (pass.instance_count > instance_count)
	{
		Log::warn(
			"Trying to draw more instances than exist in the index buffer (%i / %i); trimming extra instances",
			pass.instance_count,
			instance_count);

		pass.instance_count = instance_count;
	}

	// get the total drawable size
	auto draw_size = Vec2f(pass.target->width(), pass.target->height());

	// Validate Viewport
	if (!pass.has_viewport)
	{
		pass.viewport.x = 0;
		pass.viewport.y = 0;
		pass.viewport.w = draw_size.x;
		pass.viewport.h = draw_size.y;
	}
	else
	{
		pass.viewport = pass.viewport.overlap_rect(Rectf(0, 0, draw_size.x, draw_size.y));
	}

	// Validate Scissor
	if (pass.has_scissor)
		pass.scissor = pass.scissor.overlap_rect(Rectf(0, 0, draw_size.x, draw_size.y));

	// perform render
	Internal::app_renderer()->render(pass);
}
