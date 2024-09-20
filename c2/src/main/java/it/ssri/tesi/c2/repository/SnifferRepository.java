package it.ssri.tesi.c2.repository;

import it.ssri.tesi.c2.model.Sniffer;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface SnifferRepository extends JpaRepository<Sniffer, Integer> {
}
