/*
Created by RitsukiShuto on 2020/06/21-20:50:56.
Coding 'Shift JIS'

--�g����--
    *�����Ɏg��wav�t�@�C���A�w�b�_�t�@�C���A���̑��̈ˑ��t�@�C�������̃R�[�h�Ɠ����f�B���N�g���ɒu��
    *���s���ɂ������̃p�����[�^�𒀎���`����
    *���s���ʂ͂��ׂ�csv�t�@�C���ŋA���Ă���

--�R���p�C��--
    gcc���g�p����Bcl�ł̓R���p�C�����ʂ�Ȃ��\������

*/
#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

// prototype�錾
int readWavHead(FILE* fp, int* dataLength, unsigned long* fs, unsigned short* chNum, int* sampSize);
int writeWavHead(FILE* fp, int dataLength, unsigned long fs, unsigned short  chNum, int sampSize);
int CT_fft(double* x, double* y, int n, int fr);

void main(int argc, char *argv[]){
    // ���o�̓t�@�C��
    char ioFile[256];  // �o�̓t�@�C���̑f
    char inFile[256];   // ���̓t�@�C��
    char outFile[256];  // �o�̓t�@�C��
    char csvspctFile[256];  // CSV�t�@�C��
    char csvinfoFile[256];
    char csvvolFile[256];

    FILE *ifp, *ofp, *fp_info, *fp_spect, *fp_vol;

    // �M�������p
    int FRAMESIZE;
    int len, sampSize, outlen;
    unsigned long fs;
    unsigned short chNum;
    int fNum;

    // ���g�������p 
    double GAIN;
    double cutOff;
    int cutHz, cutHzL1, cutHzL2;
    int cut_Hz_N1, cut_Hz_N2;
    int N1, N2;

    // �R�}���h���C���������m�F
    if(argc < 2){
        puts("�R�}���h���C������������܂���");
        exit(0);
    }

	strcpy(inFile, argv[1]);    // ���̓t�@�C�����R�s�[
    strcpy(ioFile, argv[1]);

    for(int i = 0;i < 256;i++){
        // '.'������
        if(ioFile[i] == '.'){
            // �g���q�ȍ~��'\0'�Ŗ��߂�
            for(int j = i;j < 256;j++){
                ioFile[j] = '\0';
                // Null����������������I��
                if(ioFile[j + 1] == '\0'){
                    break;
                }
            }
        }
    }

    // �o�̓t�@�C�������쐬
    sprintf(outFile, "Out%s.wav", ioFile);      // sprintf:������A���p�֐�
    sprintf(csvinfoFile, "info%s.csv", ioFile);
    sprintf(csvspctFile, "PowSpect%s.csv", ioFile);
    sprintf(csvvolFile, "Vol%s.csv", ioFile);

    // �o�̓t�@�C�����m�F
	puts("�ȉ��̃t�@�C�����쐬����܂�");
    printf("%s\n", outFile);
    printf("%s\n", csvinfoFile);
    printf("%s\n", csvspctFile);
    printf("%s\n", csvvolFile);

	ifp = fopen(inFile, "rb");

	ofp = fopen(outFile, "wb");
    fp_info = fopen(csvinfoFile, "w");
    fp_spect = fopen(csvspctFile, "w");
    fp_vol = fopen(csvvolFile, "w");

    // wav�w�b�_��ǂݍ���
	if (readWavHead(ifp, &len, &fs, &chNum, &sampSize) < 0) {
		puts("���̓t�@�C���̃w�b�_�����܂��ǂ߂܂���");
		exit(-1);
	}

	// wav�p�����[�^���o��
	printf("�t�@�C���̒����� %d �T���v��\n", len);
	printf("�T���v�����O���g���� %d Hz\n", fs);
	printf("�`���l������ %d\n", chNum);
	printf("1�T���v���̃r�b�g���� %d �r�b�g\n\n", sampSize);

    // wav�p�����[�^����������
    fprintf(fp_info, "�t�@�C���̒����� %d �T���v��\n", len);
	fprintf(fp_info, "�T���v�����O���g���� %d Hz\n", fs);
	fprintf(fp_info, "�`���l������ %d\n", chNum);
	fprintf(fp_info, "1�T���v���̃r�b�g���� %d �r�b�g\n\n", sampSize);

    // ���̓t�@�C���p�����[�^���͕�
    puts("�e��p�����[�^�����");
    printf("FRAMESIZE = ");     scanf("%d", &FRAMESIZE);
    printf("GAIN = ");          scanf("%lf", &GAIN);
    printf("�������������g���ш�̍ŏ��l�����[Hz]: ");
    scanf("%d", &cutHzL1);
    printf("�������������g���ш�̍ő�l�����[Hz]: ");
    scanf("%d", &cutHzL2);

    // �����̈���v�Z
    cutOff = cutHzL1 * (FRAMESIZE / (double)fs);
    N1 = (int)cutOff + 1;       // �n�_
    cutOff = cutHzL2 * (FRAMESIZE /(double)fs);
    N2 = (int)cutOff + 1;       // �I�_

    // ���̓t�@�C���p�����[�^���m�F
    printf("FRAMESIZE: %d\n", FRAMESIZE);
    printf("GAIN: %lf\n", GAIN);
	printf("%d[Hz]����%d[Hz]������\n", cutHzL1, cutHzL2);
    printf("����Ώۂ̗v�f�ԍ���[%d]����[%d]\n", N1, N2);

    // �������� info
    fprintf(fp_info, "FRAMESIZE: %d\n", FRAMESIZE);
    fprintf(fp_info, "GAIN: %lf\n", GAIN);
	fprintf(fp_info, "%d[Hz]����%d[Hz]������\n", cutHzL1, cutHzL2);
    fprintf(fp_info, "����Ώۂ̗v�f�ԍ���[%d]����[%d]\n", N1, N2);

    //// BUG:cl�R�}���h�ŃR���p�C�����ʂ�Ȃ��ꏊ => gcc�R���p�C���Œʂ�̂Ń��V!
    short dataIn[FRAMESIZE];
	short dataOutX[FRAMESIZE], dataOutY[FRAMESIZE];
	double dDataInX[FRAMESIZE], dDataInY[FRAMESIZE];
	double dDataOutX[FRAMESIZE], dDataOutY[FRAMESIZE];

    fNum = (int)floor(len / FRAMESIZE);
	outlen = fNum * FRAMESIZE;

	writeWavHead(ofp, outlen, fs, chNum, sampSize);

    // �ȉ���������
	for (int i = 0; i < fNum; i++) {
		fread(dataIn, sizeof(short), FRAMESIZE, ifp);

        // FFT�p�̃f�[�^������
		for (int j = 0; j < FRAMESIZE; j++) {
			dDataInX[j] = (double)dataIn[j];
			dDataInY[j] = 0.0;
		}

		CT_fft(dDataInX, dDataInY, FRAMESIZE, 1); //FFT

        // FFT���ʂ��p���[�X�y�N�g���ɂ��Ċi�[
		for (int j = 0; j < FRAMESIZE; j++) {
			dDataOutX[j] = dDataInX[j];
			dDataOutY[j] = dDataInY[j];
		}
        // �Ώۂ̎��g��������()
        for(int j = 0;j < FRAMESIZE; j++){
            if(j >= N1 && j < N2){
                dDataOutX[j] = GAIN * dDataOutX[j];     // �g�`����
			    dDataOutY[j] = GAIN * dDataOutY[j];
                fprintf(fp_spect, "%lf\n", sqrt(pow(dDataOutX[j], 2) + pow(dDataOutY[j], 2)));  // PowSpect
            }else{
                dDataOutX[j] = dDataInX[j];             // �������s�v�ȐM���Ȃ̂ŉ������Ȃ�
			    dDataOutY[j] = dDataInY[j];
                fprintf(fp_spect, "%lf\n", sqrt(pow(dDataOutX[j], 2) + pow(dDataOutY[j], 2)));  // PowSpect
            }
        }

		CT_fft(dDataOutX, dDataOutY, FRAMESIZE, -1);    // IFFT(arg4 = -1)

		for (int j = 0; j < FRAMESIZE; j++) {
//			fprintf(fp_vol, "%lf,%lf\n", dDataOutX[j], dDataOutY[j]);   // �tFFT�̌���
			dataOutX[j] = (short)dDataOutX[j];
			dataOutY[j] = (short)dDataOutY[j];
			fprintf(fp_vol, "%d,%d\n", dataOutX[j], dataOutY[j]); 	    // Vol
		}

		fprintf(fp_spect, "\n");
        fprintf(fp_vol, "\n");

		fwrite(dataOutX, sizeof(short), FRAMESIZE, ofp);    // dataOutX�݂̂������o���Ă��邪�AY�̈����͏ꍇ�ɂ��
	}

	fclose(ifp);
	fclose(ofp);
    fclose(fp_info);
    fclose(fp_spect);
    fclose(fp_vol);
}