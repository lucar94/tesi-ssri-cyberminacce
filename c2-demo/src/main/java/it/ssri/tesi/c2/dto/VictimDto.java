package it.ssri.tesi.c2.dto;

import lombok.Builder;
import lombok.Data;

@Data
@Builder
public class VictimDto {
    private String id;
    private String encryptedFolder;
}
