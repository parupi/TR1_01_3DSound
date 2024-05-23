#include "Sound.h"

int Sound::Initialize()
{
	FILE* fp;
	unsigned long memory = 0, length = 0;

	fp = fopen("fanfare.wav", "rb");
	if (!fp) {
		//printf("ファイルを開けない\n");
		return 0;
	}
	//ファイルを開くときの変数
	char type[4];
	DWORD size, chunkSize;
	short formatType, channels;
	DWORD sampleRate, avgBytesPerSec;
	short bytesPerSample, bitsPerSample;
	unsigned long dataSize;

	//Check that the WAVE file is OK
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F') {
		//printf("not 'RIFF'\n");
		return 0;
	}
	fread(&size, sizeof(DWORD), 1, fp);
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E') {
		//printf("not 'WAVE'\n");
		return 0;
	}
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ') {
		//printf("not 'fmt '\n");
		return 0;
	}
	//ここからchunkSizeバイト分がwavのパラメータ領域
	fread(&chunkSize, sizeof(DWORD), 1, fp);
	//基本的にpcm形式
	fread(&formatType, sizeof(short), 1, fp);
	//wavのチャンネル数
	fread(&channels, sizeof(short), 1, fp);
	//サンプリング周波数
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	//1秒間の平均転送レート(=channels*sampleRate*bitsPerSample / 8)
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	//各サンプル数のbyte数(例えば、16bit, 2channelsなら4)
	fread(&bytesPerSample, sizeof(short), 1, fp);
	//量子化ビット数　(16, 8)
	fread(&bitsPerSample, sizeof(short), 1, fp);

	for (int i = 0; i < 256; i++) {
		fread(type, sizeof(char), 1, fp);
		if (type[0] == 'd') {
			fread(type, sizeof(char), 1, fp);
			if (type[0] == 'a') {
				fread(type, sizeof(char), 1, fp);
				if (type[0] == 't') {
					fread(type, sizeof(char), 1, fp);
					if (type[0] == 'a') {
						break;
					}
				}
			}
		}
		if (i == 255) {
			//printf("データがありません。\n");
			return 0;
		}
	}
	//波形データ長
	fread(&dataSize, sizeof(unsigned long), 1, fp);
	//1サンプルの長さ
	memory = bitsPerSample / 8;
	//どのくらい読み込むか
	length = dataSize / memory;
	signed short* wav_data = new signed short[length];

	//波形データ
	fread(wav_data, memory, length, fp);
	fclose(fp);

	//いつもの
	ALCdevice* device = alcOpenDevice(nullptr);
	ALCcontext* context = alcCreateContext(device, nullptr);
	alcMakeContextCurrent(context);
	alGenBuffers(1, &buffer);
	alGenSources(1, &source);
	//ALuint frequency = sampleRate;

	//ステレオだった場合モノラルに統合
	if (channels == 2) {
		for (unsigned int i = 0; i < length; i += 2) {
			//平均値をとる
			wav_data[i / 2] = wav_data[i] / 2 + wav_data[i + 1] / 2;
		}
	}
	//長さもモノラルに直す
	length = length / channels;
	if (bitsPerSample == 8) {
		//printf("8bitです。");
		return 0;
	}
	ALuint format = AL_FORMAT_MONO16;
	alBufferData(buffer, format, &wav_data[0], length * sizeof(signed short), sampleRate);
	alSourcei(source, AL_BUFFER, buffer);
	alSourcePlay(source);
	//バッファに格納したので消してよい
	delete[] wav_data;

	return 0;
}

void Sound::PlayAudio(const ALfloat* ListenerPos, const ALfloat* SourcePos)
{
	//リスナー(自分)を空間座標に配置
	alSourcefv(source, AL_POSITION, ListenerPos);
	alSourcei(source, AL_LOOPING, AL_TRUE);
	//設定を適用
	alSourcefv(source, AL_POSITION, SourcePos);
	Sleep(30);
}

Sound::~Sound()
{
}
