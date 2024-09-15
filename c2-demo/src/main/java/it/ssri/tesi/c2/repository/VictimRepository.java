package it.ssri.tesi.c2.repository;

import it.ssri.tesi.c2.model.Victim;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface VictimRepository extends JpaRepository<Victim, String> {
}
