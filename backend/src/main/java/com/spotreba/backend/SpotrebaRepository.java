package com.spotreba.backend;

import org.springframework.data.jpa.repository.JpaRepository;
import java.time.LocalDateTime;
import java.util.List;

public interface SpotrebaRepository extends JpaRepository<Spotreba, Long> {

    // Původní – posledních 50 záznamů
    List<Spotreba> findTop50ByOrderByDatumDesc();

    // Nový – data mezi dvěma daty
    List<Spotreba> findByDatumBetweenOrderByDatumAsc(LocalDateTime od, LocalDateTime do_);
}
