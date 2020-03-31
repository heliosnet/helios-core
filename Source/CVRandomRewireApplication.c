//
//  CVMaleabilityApplication.c
//  CVNetwork
//
//  Created by Filipi Nascimento Silva on 10/29/16.
//  Copyright © 2016 Filipi Nascimento Silva. All rights reserved.
//

#include "CVNetwork.h"

int main(int argc, char *argv[]){
	CVRandomSeedDev();
	CVString networkPath = NULL;
	CVString outputPath = NULL;
	
	CVFloat p;

	if(argc!=4){
		printf("unexpected number of arguments (%d)\n",(int)argc);
		return EXIT_FAILURE;
	}else{
		//CVRandomWalk q p w m filename.xnet
		p = strtof(argv[1],NULL);
		networkPath = argv[2];
		outputPath = argv[3];
	}
	
	// networkPath = "/Users/filipi/Dropbox/Software/CVRandomWalks/venuesNetworkBackbone.xnet";
	// outputPath = "/Users/filipi/Dropbox/Software/CVRandomWalks/venuesNetworkBackbone_sentences.txt";
	FILE* networkFile = fopen(networkPath,"r");
	if(!networkFile){
		printf("Cannot load file \"%s\". \n",networkPath);
	}
	CVNetwork* network = CVNewNetworkFromXNETFile(networkFile);
	fclose(networkFile);

	return EXIT_SUCCESS;
}


