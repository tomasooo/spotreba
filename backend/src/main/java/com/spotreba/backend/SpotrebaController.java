package com.spotreba.backend;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.format.annotation.DateTimeFormat;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.util.List;

@RestController
@RequestMapping("/api")
@CrossOrigin(origins = "*") // Pokud potřebuješ přístup z frontendu
public class SpotrebaController {

    @Autowired
    private SpotrebaRepository spotrebaRepository;

    // Původní endpoint - všechna data
    @GetMapping("/spotreba")
    public List<Spotreba> getAllSpotreba() {
        return spotrebaRepository.findAll();
    }

    // Nový endpoint - data v rozsahu času
    @GetMapping("/spotreba/range")
    public List<Spotreba> getSpotrebaByRange(
            @RequestParam("od") @DateTimeFormat(iso = DateTimeFormat.ISO.DATE_TIME) LocalDateTime od,
            @RequestParam("do") @DateTimeFormat(iso = DateTimeFormat.ISO.DATE_TIME) LocalDateTime do_
    ) {
        return spotrebaRepository.findByDatumBetweenOrderByDatumAsc(od, do_);
    }
}
