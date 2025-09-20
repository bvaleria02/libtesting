#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "histogram.h"
#include "enums.h"
#include "types.h"
#include "termColor.h"

ErrorCode plotHistogram(double *data, size_t size, uint8_t x, uint8_t y, char *name){
	if(data == NULL){
		return ERROR_NULL_POINTER;
	}

	if(size == 0 || x == 0 || y == 0){
		return ERROR_ZERO_LENGTH;
	}

	size_t chunk = sizeof(double) * x;
	double *raw = (double *)malloc(chunk);
	if(raw == NULL){
		return ERROR_MALLOC;
	}

	FILE *logTarget = stdout;

	double batchSize = size /(double) x;
	double batchCounter = 0;
	double batchAccumulator = 0;
	uint64_t index = 0;

	double minValue = 0;
	double maxValue = 0;

	double minValue2 = 0;
	double maxValue2 = 0;

	for(uint64_t i = 0; i < size; i++){
		if(index >= x){
			break;
		}

		if(minValue > data[i]){
			minValue = data[i];
		}

		if(maxValue < data[i]){
			maxValue = data[i];
		}

		if(batchCounter > batchSize){
			batchAccumulator += data[i] * (batchCounter - batchSize);
			raw[index] = batchAccumulator / (double) batchSize;

			if(minValue2 > raw[index]){
				minValue2 = raw[index];
			}

			if(maxValue2 < raw[index]){
				maxValue2 = raw[index];
			}

			index++;
			batchAccumulator = data[i] * (1 - (batchCounter - batchSize));
			continue;
		}

		batchAccumulator += data[i];
		batchCounter     += 1;
	}

	double lowerLimit = 0;
	double upperLimit = 0;

	fprintf(logTarget, "          Histogram: %s\n", name);
	fprintf(logTarget, "          Min (single): %.4le\tMax (single): %.4le\tMin (grouped): %.4le\tMax (grouped): %.4le\tBatch: %.2lf\n\n", minValue, maxValue, minValue2, maxValue2, batchSize);

	for(uint8_t i = 0; i < (y+1); i++){
		//lowerLimit = maxValue2 - (i+1) * (maxValue2 - minValue2) / (double) (y);
		lowerLimit = maxValue2 - i * (maxValue2 - minValue2) / (double) (y);
		upperLimit = maxValue2 - (i-1) * (maxValue2 - minValue2) / (double) (y);

		fprintf(logTarget, "          %.4le", lowerLimit);

		if(i == 0){
			fprintf(logTarget, "\n");
			continue;
		} else {
			fprintf(logTarget, "\t|");
		}

		for(uint8_t j = 0; j < x; j++){
			if(upperLimit >= 0 && lowerLimit <= 0){
				fprintf(logTarget, SET_STYLE(STYLE_STRIKE, COLOR_WHITE, COLOR_DEFAULT));
			}

			if(raw[j] >= lowerLimit && raw[j] <= upperLimit){
				fprintf(logTarget, SET_BG_FG(COLOR_CYAN) "%%");
			} else {
				fprintf(logTarget, SET_BG_FG(COLOR_DEFAULT)" ");
			}
		}

		fprintf(logTarget, RESET_STYLE "\n");
	}

	fprintf(logTarget, "\n");
	free(raw);

	return NO_ERROR;
}
