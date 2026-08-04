// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Deferred } from './Deferred';
import { Converter } from './Converter';
import { Transform } from './Transform';
import { Chain } from './Chain';
import { Pipeline } from './Pipeline';

class UpperCase implements Converter<string, string> {
    resolve(value: string): string {
        return value.toUpperCase();
    }
}

class Trim implements Transform<string> {
    apply(value: string): string {
        return value.trim();
    }
}

describe('Deferred', () => {
    describe('factory', () => {
        it('returns a Converter when given a Converter constructor', () => {
            const d = Deferred(UpperCase);
            assert.equal(d.resolve('hello', {}), 'HELLO');
        });

        it('returns a Transform when given a Transform constructor', () => {
            const d = Deferred(Trim);
            assert.equal(d.apply('  hello  ', {}), 'hello');
        });

        it('throws when given a constructor that is neither a Converter nor a Transform', () => {
            class NotAConverter {}
            assert.throws(() => Deferred(NotAConverter as any), /neither a Converter nor a Transform/);
        });
    });

    describe('Converter', () => {
        it('delegates resolve() to the constructed converter', () => {
            const d = new Deferred.Converter(UpperCase);
            assert.equal(d.resolve('hello', {}), 'HELLO');
            assert.equal(d.resolve('world', {}), 'WORLD');
        });

        it('acquire() resolves via the constructed converter', () => {
            const d = new Deferred.Converter(UpperCase);
            assert.equal(d.acquire('hello', {}), 'HELLO');
        });

        it('is assignable to Converter<T, U>', () => {
            const d: Converter<string, string> = new Deferred.Converter(UpperCase);
            assert.equal(d.resolve('hi', {}), 'HI');
        });

        it('can be installed in a Chain', () => {
            const chain = new Chain<string, string | null>();
            chain.install(1, 'upper', Deferred(UpperCase));
            assert.equal(chain.resolve('hello', {}), 'HELLO');
            assert.equal(chain.resolve('world', {}), 'WORLD');
        });
    });

    describe('Transform', () => {
        it('delegates apply() to the constructed transform', () => {
            const d = new Deferred.Transform(Trim);
            assert.equal(d.apply('  hello  ', {}), 'hello');
            assert.equal(d.apply('  world  ', {}), 'world');
        });

        it('acquire() applies the transform', () => {
            const d = new Deferred.Transform(Trim);
            assert.equal(d.acquire('  hello  ', {}), 'hello');
        });

        it('is assignable to Transform<T>', () => {
            const d: Transform<string> = new Deferred.Transform(Trim);
            assert.equal(d.apply('  hi  ', {}), 'hi');
        });

        it('can be installed in a Pipeline', () => {
            const pipeline = new Pipeline<string>();
            pipeline.install(1, 'trim', Deferred(Trim));
            assert.equal(pipeline.apply('  hello  ', {}), 'hello');
            assert.equal(pipeline.apply('  world  ', {}), 'world');
        });
    });
});
