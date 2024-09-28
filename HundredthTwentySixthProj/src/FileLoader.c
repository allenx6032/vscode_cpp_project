#include "FileLoader.h"

void DelMesh(Mesh* m)
{
	free(m->Indices);
	free(m->vertices);
	free(m);
}
int equals(const vec3i a,const vec3i b)
{
	
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

int LoadOBJModel(const char* fileN, Mesh* meshO)
{
	FILE* file;
		

	//if (fopen_s(&file, fileN, "r") != 0)
	file = fopen(fileN, "r");
	if(file == NULL)
	{
		printf("File opening error! %s\n",fileN);
		return 0;
	}
	vec3* Positions = malloc(sizeof(vec3));
	vec2* UVs = malloc(sizeof(vec2));
	vec3* Normals = malloc(sizeof(vec3));
	vec3i* Vertices = malloc(sizeof(vec3i));

	char* word = (char*)malloc(10000);
	//char* word1 = (char*)malloc(0);

	int vCnt, vtCnt, vnCnt, fCnt;
	vCnt = vtCnt = vnCnt = fCnt = 0;

	Mesh mesh;//(Mesh*)malloc(sizeof(Mesh));
	mesh.atr = 0;

	while (!feof(file))//Read until the end of file
	{
		fgets(word, 3, file);//Get the beginning of the line

		if (word[0] == 'v' && word[1] == ' ')
		{
			vCnt++;
			mesh.atr |= POSITION;
			//Get the rest of the line

			fgets(word, 10000, file);
			//Find the spaces
			int len = strlen(word);
			int spc0 = 0, spc1 = 0;
			for (size_t i = 0; i < len; i++)
				if (word[i] == ' ')
					if (spc0 == 0)
						spc0 = i;
					else
						spc1 = i;
			//extend the array
			Positions = realloc(Positions, sizeof(vec3) * vCnt);
			//Copy the needed part of the line
			char* word1 = (char*)malloc(spc0);
			memcpy(word1, word, spc0);
			//Parse
			Positions[vCnt - 1].x = atof(word1);
			free(word1);
			word1 = (char*)malloc(spc1 - spc0);
			memcpy(word1, &word[spc0], spc1 - spc0);
			Positions[vCnt - 1].y = atof(word1);
			free(word1);
			word1 = (char*)malloc(len - spc1);
			memcpy(word1, &word[spc1], len - spc1);
			Positions[vCnt - 1].z = atof(word1);
			//	Positions[vCnt - 1].c ='\0';
			free(word1);

			if (Positions[vCnt - 1].x > 20)
				printf("MORE THAN 20!!!");

			continue;
		}
		if (word[0] == 'v' && word[1] == 't')
		{
			vtCnt++;
			mesh.atr |= UV;
			//Get the rest of the line
			fgets(word, 10000, file);
			//Find the spaces
			int len = strlen(word);
			int spc0 = 0;
			for (size_t i = 0; i < len; i++)
				if (word[i] == ' ')
					if (spc0 == 0)
						spc0 = i;

			UVs = realloc(UVs, sizeof(vec2) * vtCnt);
			char* word1 = (char*)malloc(spc0);
			memcpy(word1, word, spc0);
			UVs[vtCnt - 1].x = atof(word1);
			free(word1);
			word1 = (char*)malloc(len - spc0);
			memcpy(word1, &word[spc0], len - spc0);
			UVs[vtCnt - 1].y = atof(word1);
			//UVs[vtCnt - 1].c = '\0';
			free(word1);
			continue;
		}
		if (word[0] == 'v' && word[1] == 'n')
		{
			vnCnt++;
			mesh.atr |= NORMAL;
			fgets(word, 10000, file);
			//Find the spaces
			int len = strlen(word);
			int spc0 = 0, spc1 = 0;
			for (size_t i = 0; i < len; i++)
				if (word[i] == ' ')
					if (spc0 == 0)
						spc0 = i;
					else
						spc1 = i;
			//extend the array
			Normals = realloc(Normals, sizeof(vec3) * vnCnt);
			//Copy the needed part of the line
			char* word1 = (char*)malloc(spc0);
			memcpy(word1, word, spc0);
			//Parse
			Normals[vnCnt - 1].x = atof(word1);
			free(word1);
			word1 = (char*)malloc(spc1 - spc0);
			memcpy(word1, &word[spc0], spc1 - spc0);
			Normals[vnCnt - 1].y = atof(word1);
			free(word1);
			word1 = (char*)malloc(len - spc1);
			memcpy(word1, &word[spc1], len - spc1);
			Normals[vnCnt - 1].z = atof(word1);
			//Normals[vnCnt - 1].c = '\0';

			free(word1);
			continue;
		}
		if (word[0] == 'f' && word[1] == ' ')
		{
			if (mesh.atr != ALL) goto end;//TODO implement other mesh support
			//Vertex format is Position/Texture/Normal
			fgets(word, 10000, file);
			//Get the array of spaces
			int len = strlen(word);
			int* spc = (int*)malloc(sizeof(int));
			int spcCnt = 0;
			for (size_t i = 0; i < len; i++)
				if (word[i] == ' ')
				{
					spcCnt++;
					spc = realloc(spc, sizeof(int) * spcCnt);
					spc[spcCnt - 1] = i;
				}
			spc = realloc(spc, sizeof(int) * (spcCnt + 1));
			spc[spcCnt] = len - 1;
			//spcCnt = number of indices in the line
			spcCnt++;

			//Temporary list of indices
			vec3i* tmpV = (vec3i*)calloc(sizeof(vec3i), spcCnt);

			//Special case for the first one
			char* word1 = (char*)malloc(spc[0]);
			memcpy(word1, word, spc[0]);
			int slsh0 = 0, slsh1 = 0;
			for (size_t i = 0; i < spc[0]; i++)
			{
				if (word1[i] == '/')
					if (slsh0 == 0)
						slsh0 = i;
					else
						slsh1 = i;
			}
			char* word2 = (char*)malloc(slsh0);
			memcpy(word2, word1, slsh0);
			tmpV[0].x = atoi(word2);
			free(word2);
			word2 = (char*)malloc(slsh1 - slsh0 - 1);
			memcpy(word2, &word1[slsh0 + 1], slsh1 - slsh0 - 1);
			tmpV[0].y = atoi(word2);
			free(word2);
			word2 = (char*)malloc(spc[0] - slsh1 - 1);
			memcpy(word2, &word1[slsh1 + 1], spc[0] - slsh1 - 1);
			tmpV[0].z = atoi(word2);
			free(word2);
			free(word1);

			for (size_t i = 1; i < spcCnt; i++)
			{
				int l = spc[i] - spc[i - 1] - 1;
				word1 = (char*)malloc(l);
				memcpy(word1, &word[spc[i - 1] + 1], l);
				slsh0 = 0, slsh1 = 0;
				for (size_t j = 0; j < l; j++)
				{
					if (word1[j] == '/')
						if (slsh0 == 0)
							slsh0 = j;
						else
							slsh1 = j;
				}
				char* word2 = (char*)malloc(slsh0);
				memcpy(word2, word1, slsh0);
				tmpV[i].x = atoi(word2);
				free(word2);
				word2 = (char*)malloc(slsh1 - slsh0 - 1);
				memcpy(word2, &word1[slsh0 + 1], slsh1 - slsh0 - 1);
				tmpV[i].y = atoi(word2);
				free(word2);
				word2 = (char*)malloc(l - slsh1 - 1);
				memcpy(word2, &word1[slsh1 + 1], l - slsh1 - 1);
				tmpV[i].z = atoi(word2);
				free(word2);
				free(word1);
			}
			//Triangulate if needed
			if (spcCnt > 3)
			{
				vec3i* tmpV1 = tmpV;
				tmpV = (vec3i*)malloc(sizeof(vec3i) * ((spcCnt - 2) * 3));
				size_t iter = 0;
				for (size_t i = 2; i < spcCnt; i++)
				{
					tmpV[iter] = tmpV1[0];
					iter++;
					tmpV[iter] = tmpV1[i - 1];
					iter++;
					tmpV[iter] = tmpV1[i];
					iter++;
				}
				free(tmpV1);
				spcCnt = (spcCnt - 2) * 3;
			}
			fCnt += spcCnt;
			Vertices = realloc(Vertices, sizeof(vec3i) * fCnt);
			memcpy(&Vertices[fCnt - spcCnt], tmpV, spcCnt * sizeof(vec3i));

			free(tmpV);
			free(spc);

			continue;
		}
		fgets(word, 10000, file);
	}

end:
	free(word);
	fclose(file);
	
	mesh.vertices = malloc(sizeof(Vertex));
	mesh.numVertices = 0;
	mesh.Indices = malloc(sizeof(unsigned int) * fCnt);
	mesh.numIndices = fCnt;
	//Additional temp array for vertex reduplication
	vec3i* tmpIND = malloc(fCnt * sizeof(vec3i));
	//Iterate thru indices
	for (size_t i = 0; i < fCnt; i++)
	{
		int found = 0, j = 0;
		//Try to find vertex in the mesh
		while (!found && j < mesh.numVertices)
		{
			found = equals(Vertices[i], tmpIND[i]);
			j++;
		}
		//Add the vertex if not found
		if (!found)
		{
			tmpIND[i] = Vertices[i];
			mesh.numVertices++;
			mesh.vertices = realloc(mesh.vertices, mesh.numVertices * sizeof(Vertex));
			Vertex v;
			v.Position = Positions[Vertices[i].x - 1];
			v.UV = UVs[Vertices[i].y - 1];
			v.Normal = Normals[Vertices[i].z - 1];
			mesh.vertices[mesh.numVertices - 1] = v;
			mesh.Indices[i] = mesh.numVertices - 1;
		}
		else
			mesh.Indices[i] = j;
	}

	free(tmpIND);
	free(Positions);
	free(UVs);
	free(Normals);
	free(Vertices);
	(*meshO) = mesh;
	return 1;
}

void SaveMDL(Mesh m, const char* path)
{
	FILE* f;
	f = fopen(path,"wb");
	//if (fopen_s(&f, path, "wb") != 0)
	if(f == NULL)
	{
		printf("File opening error! %s\n",path);
		return;
	}

	MDLHeader h;
	memcpy(h.signature, "_LUNAR_MODEL_FORMAT_", 21);
	h.vCnt = m.numVertices;
	h.iCnt = m.numIndices;
	h.ATR = m.atr;
	fwrite(&h, sizeof(MDLHeader), 1, f);
	fwrite(m.vertices, sizeof(Vertex), m.numVertices, f);

	fwrite(m.Indices, sizeof(unsigned int), m.numIndices, f);

	fclose(f);
}

int LoadMDL(const char* path, Mesh* meshO)
{
	FILE* f;
	f = fopen(path,"rb");
	//if (fopen_s(&f, path, "rb") != 0)
	if(path == NULL)
	{
		printf("File opening error! %s \n",path);
		return 0;
	}
	MDLHeader h;
	if (!fread(&h, sizeof(MDLHeader), 1, f))
	{
		printf("File reading error'n");
		return 0;
	}
	Mesh m;//= malloc(sizeof(Mesh));

	m.atr = h.ATR;
	m.numIndices = h.iCnt;
	m.numVertices = h.vCnt;
	m.vertices = malloc(m.numVertices * sizeof(Vertex));

	size_t cnt = fread(m.vertices, sizeof(Vertex), m.numVertices, f);

	if (cnt != m.numVertices)
	{
		printf("File reading error'n");
		return 0;
	}
	m.Indices = malloc(m.numIndices * sizeof(unsigned int));

	cnt = fread(m.Indices, sizeof(unsigned int), m.numIndices, f);

	if (cnt != m.numIndices)
	{
		printf("File reading error'n");
		return 0;
	}
	fclose(f);
//	 printf("Model pre return:\natr = %i\nnumInd = %i\nnumVert = %i\n",m.atr,m.numIndices,m.numVertices);

	*meshO = m;
	return 1;
}

unsigned char* LoadBMP(const char* file,int* h,int* w, unsigned char * bpp,int switchRB)
{
	FILE* f;
	f = fopen(file,"rb");
	//if (fopen_s(&f, file, "rb") != 0)
	if(f == NULL){
		printf("Could not open file %s\n",file);
		return 0;}
	BMPheader info;
	if (fread(&info, sizeof(BMPheader), 1, f) != 1) 
	{
		fclose(f);
		printf("Could not read header\n");
		return 0;
	}
	if(info.DIBsize != 40)
	{
		printf("Wrong bmp info");
		fclose(f);
		return 0;
	}

	bmpInfo header;
	if (fread(&header, sizeof(header), 1, f) != 1)
	{
		fclose(f);
		printf("Could not read info\n");
		return 0;
	}

	if (header.compression != 0 || info.offset != (header.palette * 5) + 54)
	{
		printf("Wrong bmp header\n");
		fclose(f);
		return 0;
	}
	fseek(f, sizeof(unsigned int) * header.palette, SEEK_CUR);//Skip the palette
	*h = header.height;
	*w = header.width;
	*bpp = header.bpp;
	unsigned int RowSize = (header.bpp * header.width) >> 3; // divided by 8 (should work (I hope))
	unsigned int PixelArraySize = RowSize * header.height;
	
	unsigned char* pixels = malloc(header.imgSize);

	if(fread(pixels,1,header.imgSize,f)!=header.imgSize)
	{
		printf("File reading error\n");
		fclose(f);
		return 0;
	}
	fclose(f);
	
	if (!switchRB)return pixels;
	unsigned char* o = malloc(header.imgSize);
	for (size_t k = 0; k < header.imgSize; k+=4)
	{
		o[k + 0] = pixels[k + 2];
		o[k + 1] = pixels[k + 1];
		o[k + 2] = pixels[k + 0];
		o[k + 3] = pixels[k + 3];
	}
	free(pixels);
	return o;
}
